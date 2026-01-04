#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#define BUZZER 15
#define TRIG 32
#define ECHO 33

// WiFi AP credentials
const char* ssid = "ESP32-Attendance";
const char* password = "12345678";

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Flash memory
Preferences prefs;

// Web server
WebServer server(80);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {18, 5, 4, 25};
byte colPins[COLS] = {16, 17, 26, 27};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// IDs
String validIDs[] = {"1234", "2345", "3456"};
const int studentCount = 3;

String input = "";
int totalCount = 0;

// ------------------ FUNCTIONS ------------------

void beep(int t) {
  digitalWrite(BUZZER, HIGH);
  delay(t);
  digitalWrite(BUZZER, LOW);
}

long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, 30000) * 0.034 / 2;
}

// Web page
void handleRoot() {
  String page = "<h2>Attendance Dashboard</h2>";
  page += "<p>Total Present: " + String(totalCount) + "</p><hr>";

  for (int i = 0; i < studentCount; i++) {
    page += validIDs[i] + " : ";
    page += prefs.getBool(validIDs[i].c_str(), false) ? "Present<br>" : "Absent<br>";
  }

  server.send(200, "text/html", page);
}

// ------------------ SETUP ------------------

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Wire.begin(23, 19);
  lcd.init();
  lcd.backlight();

  prefs.begin("attendance", false);
  totalCount = prefs.getInt("count", 0);

  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();

  lcd.print("Smart Attendance");
  delay(2000);
  lcd.clear();
  lcd.print("Enter ID:");
}

// ------------------ LOOP ------------------

void loop() {
  server.handleClient();

  char key = keypad.getKey();
  if (!key) return;

  beep(40);

  // SUBMIT
  if (key == '#') {
    lcd.clear();

    // 🔐 ADMIN RESET (FIXED)
    if (input == "0000") {
      prefs.clear();
      prefs.putInt("count", 0);
      totalCount = 0;

      lcd.print("Memory Cleared");
      beep(600);
      delay(2000);

      input = "";
      lcd.clear();
      lcd.print("Enter ID:");
      return;
    }

    // 👤 ANTI-PROXY CHECK
    long dist = getDistance();
    if (dist > 40 || dist == 0) {
      lcd.print("No Person!");
      beep(500);
    } 
    else {
      bool valid = false;

      for (int i = 0; i < studentCount; i++) {
        if (input == validIDs[i]) {
          valid = true;

          if (!prefs.getBool(input.c_str(), false)) {
            prefs.putBool(input.c_str(), true);
            totalCount++;
            prefs.putInt("count", totalCount);

            lcd.print("Attendance OK");
            beep(200);
          } 
          else {
            lcd.print("Already Marked");
            beep(300);
          }
        }
      }

      if (!valid) {
        lcd.print("Invalid ID");
        beep(400);
      }
    }

    delay(2000);
    input = "";
    lcd.clear();
    lcd.print("Enter ID:");
  }

  // CLEAR INPUT
  else if (key == '*') {
    input = "";
    lcd.clear();
    lcd.print("Cleared");
    delay(1000);
    lcd.clear();
    lcd.print("Enter ID:");
  }

  // DIGITS
  else {
    input += key;
    lcd.setCursor(0, 1);
    lcd.print("****");
  }
}
