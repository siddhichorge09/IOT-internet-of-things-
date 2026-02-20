#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

// 👉 replace with YOUR UID
byte allowedUID[4] = {0x85, 0x7E, 0x3F, 0x02};

bool locked = true;
unsigned long lastMsg = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("RFID System");
  delay(1500);
  lcd.clear();

  servo.attach(3);
  servo.write(70);

  Serial.println("System Ready");
}

void showIdle() {
  lcd.setCursor(0,0);
  lcd.print("Scan your card ");
}

void loop() {

  // show idle message every 1 sec only
  if (millis() - lastMsg > 1000) {
    lcd.clear();
    showIdle();
    lastMsg = millis();
  }

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  lcd.clear();
  lcd.print("Card Detected");

  bool match = true;
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedUID[i]) {
      match = false;
      break;
    }
  }

  delay(700);
  lcd.clear();

  if (match) {
    if (locked) {
      servo.write(200);
      lcd.print("Door OPEN");
      locked = false;
      Serial.println("OPEN");
    } else {
      servo.write(70);
      lcd.print("Door LOCKED");
      locked = true;
      Serial.println("LOCKED");
    }
  } else {
    lcd.print("Wrong Card");
    Serial.println("DENIED");
  }

  delay(2000);
  lcd.clear();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
