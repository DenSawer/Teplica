#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>

#define MODEM_RX 16
#define MODEM_TX 17

HardwareSerial sim800(1);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int timeSyncInterval = 360;  // Интервал синхронизации в секундах
bool timeSynced = false;
time_t lastSyncTime = 0;
time_t lastDisplayedTime = 0;

namespace SIM {  // область команд для сим
bool initModule() {
  sim800.println("AT");
  delay(100);
  return sim800.find("OK");
}

bool initSim() {
  sim800.println("AT+CPIN?");
  delay(100);
  return sim800.readString().indexOf("READY") != -1;
}
}

void getTimeFromSIM() {
  sim800.println("AT+CCLK?");
  delay(100);
  String response = sim800.readString();
  time_t newTime;
  int year, month, day, hour, minute, second;
  if (sscanf(response.c_str(), "\r\n+CCLK: \"%d/%d/%d,%d:%d:%d\"", &year, &month, &day, &hour, &minute, &second) == 6) {
    struct tm timeinfo = {};
    timeinfo.tm_year = year + 100;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    newTime = mktime(&timeinfo);
  }
  if (newTime > 1670000000) {  // Проверяем, что время реальное, а не дефолтное
    struct timeval tv = { newTime, 0 };
    settimeofday(&tv, NULL);
    lastSyncTime = newTime;
    timeSynced = true;
  } else timeSynced = false;
}

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
  sim800.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  lcd.init();
  lcd.backlight();

  if (!SIM::initModule() || !SIM::initSim()) {
    Serial.println("Нет сим");
    return;
  }
  getTimeFromSIM();
}

void loop() {
  static time_t lastSecond = 0;
  time_t now = time(nullptr);

  if (!timeSynced || now - lastSyncTime >= timeSyncInterval) {
    getTimeFromSIM();
  }

  if (now != lastSecond) {  // Обновление только при смене секунды
    lastSecond = now;
    printLocalTime();
  }
}
