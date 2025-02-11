#include <WiFi.h>
#include <time.h>
#include "LiquidCrystal_I2C.h"  // LCD по I2C

#define LCD_ADDRESS 0x27
#define LCD_COLUMN 16
#define LCD_ROW 2
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMN, LCD_ROW);  // Временная инициализация для проверки

const char* ssid = "POCO X6 5G";
const char* password = "123456789";
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 10800;  // Пример UTC+3
int daylightOffset_sec = 0;

// Подключение к Wi-Fi
void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

// Настройка времени через NTP
void setupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

// Вывод текущего времени
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("Current time: ");
  Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
  lcd.setCursor(0, 0);
  lcd.print(&timeinfo, "%Y-%m-%d");
  lcd.setCursor(0, 1);
  lcd.print(&timeinfo, "%H:%M:%S");
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  setupWiFi();
  setupTime();
}

void loop() {
  static unsigned long lastSyncTime = 0;
  if (millis() - lastSyncTime >= 15000) {  // Обновление времени раз в час
    setupTime();
    lastSyncTime = millis();
    Serial.println("Time synchronized");
  }
  printLocalTime();
  delay(1000);  // Вывод времени каждые 10 секунд
}
