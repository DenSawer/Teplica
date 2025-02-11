#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <time.h>

#define MODEM_RX 16
#define MODEM_TX 17
#define LCD_ADDRESS 0x27
#define LCD_COLUMN 16
#define LCD_ROW 2

HardwareSerial sim800(1);
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMN, LCD_ROW);

const char* ssid = "POCO X6 5G";
const char* password = "123456789";
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 10800;
int daylightOffset_sec = 0;

bool useSIM = false;
time_t lastSyncTime = 0;
const int timeSyncInterval = 15;

void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int retries = 20;
  while (WiFi.status() != WL_CONNECTED && retries > 0) {
    delay(1000);
    Serial.print(".");
    retries--;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected!");
  } else {
    Serial.println(" Failed to connect to WiFi");
  }
}

void setupTimeNTP() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  lastSyncTime = time(nullptr);
}

bool initSIM() {
  sim800.println("AT");
  delay(100);
  if (!sim800.find("OK")) return false;
  sim800.println("AT+CPIN?");
  delay(100);
  return sim800.readString().indexOf("READY") != -1;
}

void getTimeFromSIM() {
  sim800.println("AT+CCLK?");
  delay(100);
  String response = sim800.readString();
  int year, month, day, hour, minute, second;
  if (sscanf(response.c_str(), "\r\n+CCLK: \"%d/%d/%d,%d:%d:%d\"", &year, &month, &day, &hour, &minute, &second) == 6) {
    struct tm timeinfo = {};
    timeinfo.tm_year = year + 100;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    time_t newTime = mktime(&timeinfo);
    if (newTime > 1670000000) {
      struct timeval tv = { newTime, 0 };
      settimeofday(&tv, NULL);
      lastSyncTime = newTime;
    }
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  lcd.setCursor(0, 0);
  lcd.printf("%04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
  lcd.setCursor(0, 1);
  lcd.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  lcd.init();
  lcd.backlight();

  if (initSIM()) {
    Serial.println("Using SIM800L for time sync");
    useSIM = true;
    getTimeFromSIM();
  } else {
    Serial.println("No SIM detected, using WiFi");
    setupWiFi();
    setupTimeNTP();
  }
}

void loop() {
  time_t now = time(nullptr);
  if (!timeSynced || now - lastSyncTime >= timeSyncInterval) {
    if (useSIM) {
      getTimeFromSIM();
    } else {
      setupTimeNTP();
    }
  }
  printLocalTime();
  delay(1000);
}
