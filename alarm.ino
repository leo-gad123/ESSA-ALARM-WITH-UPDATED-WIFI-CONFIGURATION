#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>   

#define ALARM_PIN D5
#define WIFI_RESET_PIN D3   

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 2 * 3600, 60000);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiManager wm;



void ringAlarm(int durationSec) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("It's Time");
  digitalWrite(ALARM_PIN, HIGH);
  delay(durationSec * 1000);
  digitalWrite(ALARM_PIN, LOW);
  lcd.clear();
}

bool shouldRing(int dayOfWeek, int hour, int minute, int second) {
  if (dayOfWeek >= 1 && dayOfWeek <= 5) {
    if ((hour == 8 && minute == 40 && second == 0) ||
        (hour == 9 && minute == 20 && second == 0) ||
        (hour == 10 && minute == 0 && second == 0) ||
        (hour == 11 && minute == 50 && second == 0) ||
        (hour == 12 && minute == 30 && second == 0) ||
        (hour == 14 && minute == 50 && second == 0) ||
        (hour == 15 && minute == 30 && second == 0)) {
      ringAlarm(6);
      return true;
    }

    if ((hour == 8 && minute == 0 && second == 0) ||
        (hour == 10 && minute == 40 && second == 0) ||
        (hour == 11 && minute == 10 && second == 0) ||
        (hour == 13 && minute == 10 && second == 0) ||
        (hour == 14 && minute == 10 && second == 0) ||
        (hour == 16 && minute == 10 && second == 0) ||
        (hour == 18 && minute == 0 && second == 0) ||
        (hour == 19 && minute == 0 && second == 0) ||
        (hour == 21 && minute == 0 && second == 0)) {
      ringAlarm(15);
      return true;
    }

    if ((dayOfWeek == 1 || dayOfWeek == 3 || dayOfWeek == 5) &&
        hour == 7 && minute == 30 && second == 0) {
      ringAlarm(15);
      return true;
    }
  }

  if (dayOfWeek == 7 || dayOfWeek == 6) {
    if ((hour == 7 && minute == 10 && second == 0) ||
        (hour == 12 && minute == 45 && second == 0) ||
        (hour == 18 && minute == 0 && second == 0)) {
      ringAlarm(15);
      return true;
    }
  }
  return false;
}



void setup() {
  Serial.begin(115200);

  pinMode(ALARM_PIN, OUTPUT);
  pinMode(WIFI_RESET_PIN, INPUT_PULLUP);
  digitalWrite(ALARM_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.print("Starting...");

  
  if (digitalRead(WIFI_RESET_PIN) == LOW) {
    lcd.clear();
    lcd.print("WiFi Reset");
    wm.resetSettings();
    delay(1500);
  }

  
  if (!wm.autoConnect("ESSA_Alarm")) {
    ESP.restart();
  }

  lcd.clear();
  lcd.print("WiFi Connected");
  delay(1500);

  timeClient.begin();
}



void loop() {
  timeClient.update();

  int dayOfWeek = timeClient.getDay();
  if (dayOfWeek == 0) dayOfWeek = 7;

  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();

  const char* days[] = {
    "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday", "Sunday"
  };

  lcd.setCursor(0, 0);
  lcd.print("Day: ");
  lcd.print(days[dayOfWeek - 1]);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  char timeStr[17];
  snprintf(timeStr, sizeof(timeStr),
           "Time:%02d:%02d:%02d", hour, minute, second);
  lcd.print(timeStr);

  Serial.println(timeStr);

  shouldRing(dayOfWeek, hour, minute, second);
  delay(1000);
}
