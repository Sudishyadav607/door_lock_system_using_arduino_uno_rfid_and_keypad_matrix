#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 15
#define RST_PIN 14
#define statePin 16
#define servoPin 10
#define servo_close 10
#define servo_open 200
const int Password_Length = 7;
int timey;

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

char Data[Password_Length + 1]; // +1 for null terminator
bool matchFound = false;
byte data_count = 0;
int failedAttempts = 0;
const int maxAttempts = 3;

byte colPins[COLS] = {5, 4, 3, 2};
byte rowPins[ROWS] = {9, 8, 7, 6};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

struct User {
  char username[10];
  char pin[8];
  char password[8];
  String uid;
};
User authorizedUsers[] = {
    {"USER1", "1234567", "PASSWO1", "33B0B9D9"},
    {"USER2", "9876543", "PASSWO2", ""},
    {"USER3", "7654321", "PASSWO3", ""},
    {"USER4", "2345678", "PASSWO4", ""},
    {"USER5", "3456789", "PASSWO5", "2C7B973F"},
};
const int numUsers = sizeof(authorizedUsers) / sizeof(authorizedUsers[0]);
User currentUser; // Store the current user logged in

void setup() {
  timey = 10000;
  pinMode(statePin, INPUT);
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  Serial.println("Ready for action");
  myServo.attach(servoPin);
  myServo.write(servo_close);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("Welcome");
  
  clearData();
}

void loop() { 
   handleKeypadInput();
  handleSerialInput();
  
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Put your card");
  card();
  }
}

void handleKeypadInput() {
  char customKey = customKeypad.getKey();
  if (customKey && data_count < Password_Length) {
    lcd.setCursor(4, 0);
    lcd.print("Enter pin");
    lcd.setCursor(data_count, 1);
    lcd.print('*'); // Hide actual input
    Data[data_count] = customKey;
    data_count++;

    if (data_count == Password_Length) {
      keyPad();
    }
  }
}

void handleSerialInput() {
  int state = digitalRead(statePin);
  if (state == HIGH) {
    if (Serial.available() > 0) {
      String serialInput = Serial.readString();
      serialInput.trim();
      Serial.print("Input is:->");
      Serial.println(serialInput);
      
      if (serialInput.length() == Password_Length) {
        matchFound = false;
        
        for (int i = 0; i < numUsers; i++) {
          if (serialInput.equals(authorizedUsers[i].password)) {
            currentUser = authorizedUsers[i];
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Welcome ");
            lcd.print(currentUser.username);
            open();
            matchFound = true;
            break;
          }
        }

        if (!matchFound) {
          close();
        }
      } else {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Invalid Input");
        close();
      }
    }
  }
}

void card() {
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Put your card");


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning");
  Serial.print("NUID tag is: ");
  
  String ID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    lcd.print(".");
    ID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    ID += String(rfid.uid.uidByte[i], HEX);
    delay(300);
  }
  ID.toUpperCase();
  Serial.println(ID);

  matchFound = false;
  
  for (int i = 0; i < numUsers; i++) {
    if (ID == authorizedUsers[i].uid) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome ");
      lcd.print(authorizedUsers[i].username);
      currentUser = authorizedUsers[i];
      open();
      matchFound = true;
      break;
    }
  }
  
  if (!matchFound) {
    Serial.println("Access denied.");
    close();
  }
}

void open() {
  myServo.write(servo_open);
  Serial.println("Access granted");
  failedAttempts = 0;
  delay(2000);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Welcome");
  logOut();
}

void close() {
  myServo.write(servo_close);
  Serial.println("Access denied. Wrong password.");
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Wrong Password");
  
  handleFailedLogin();
  delay(1000);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Welcome");
}

void clearData() {
  data_count = 0;
  memset(Data, 0, sizeof(Data));
  matchFound = false;
}

void logOut() {
  delay(timey);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Logout....");
  myServo.write(servo_close);
}

void keyPad() {
  Serial.print("Input is: ");
  Serial.println(Data);
  Data[Password_Length] = '\0';
  
  matchFound = false;
  
  for (int i = 0; i < numUsers; i++) {
    if (strcmp(Data, authorizedUsers[i].pin) == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome ");
      lcd.print(authorizedUsers[i].username);
      currentUser = authorizedUsers[i];
      open();
      matchFound = true;
      break;
    }
  }
  
  if (!matchFound) {
    close();
  }
  
  clearData();
}

void handleFailedLogin() {
  failedAttempts++;
  if (failedAttempts >= maxAttempts) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Locked for 10s");
    Serial.println("Locked for 10s");
    
    for (int i = 10; i > 0; i--) {
      lcd.setCursor(1, 1);
      lcd.print("Unlocking in: ");
      lcd.print(i);
      Serial.println(i);
      delay(1000);
    }
    
    Serial.println("Unlocked");
    failedAttempts = 0;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Welcome");
  }
}