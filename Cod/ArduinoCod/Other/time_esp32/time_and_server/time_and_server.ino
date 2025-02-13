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
long gmtOffset_sec = 10800;
int daylightOffset_sec = 0;

time_t lastSyncTime = 0;
const int timeSyncInterval = 3600;  // Интервал синхронизации времени в секундах

// Подключение к WiFi
void setupWiFi() {
  WiFi.begin(ssid, password);
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    delay(1000);
  }
}

// Настройка времени через NTP
bool setupTimeNTP() {
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
  const time_t currentTime = time(nullptr);
  // Если время корректное (порог можно заменить на нужное значение)
  if (currentTime > 1670000000UL) {
    lastSyncTime = currentTime;
    return true;
  }
  return false;
}

// Получение времени с SIM800L
bool getTimeFromSIM() {
  sim800.println("AT+CCLK?");
  delay(100);
  const String response = sim800.readString();

  // Используем uint16_t для года и uint8_t для остальных компонентов
  uint16_t year = 0;
  uint8_t month = 0, day = 0, hour = 0, minute = 0, second = 0;

  // Используем соответствующие спецификаторы формата:
  // %hu для unsigned short (uint16_t) и %hhu для unsigned char (uint8_t)
  if (sscanf(response.c_str(), "\r\n+CCLK: \"%hu/%hhu/%hhu,%hhu:%hhu:%hhu\"", &year, &month, &day, &hour, &minute, &second) == 6) {
    struct tm timeinfo = { 0 };
    // Предполагается, что SIM возвращает год в формате, например, 21 для 2021 года.
    // В структуре tm год хранится как количество лет с 1900, поэтому прибавляем 100.
    timeinfo.tm_year = year + 100;
    timeinfo.tm_mon = month - 1;  // месяцы от 0 до 11
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;

    const time_t newTime = mktime(&timeinfo);
    if (newTime > 1670000000UL) {  // Проверка корректности времени
      struct timeval tv = { newTime, 0 };
      settimeofday(&tv, NULL);
      lastSyncTime = newTime;
      return true;
    }
  }
  return false;
}

// Обновление локального времени (проверка, пора ли обновлять)
void updateLocalTime() {
  const time_t currentTime = time(nullptr);
  if (time(nullptr) - lastSyncTime < timeSyncInterval) return; // Проверка прошло ли уже время обновиться
  if (getTimeFromSIM()) return; // Обноление через сим
  if (setupTimeNTP()) return; // Обноление через нтп, то есть интернет 
  lastSyncTime = time(nullptr);// Если обе попытки неудачны, обновляем lastSyncTime с локального времени
}

// Вывод локального времени на LCD дисплей
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("Current time: ");
  Serial.println(&timeinfo, "%Y/%m/%d %H:%M:%S");
  lcd.setCursor(0, 0);
  lcd.print(&timeinfo, "%Y/%m/%d");
  lcd.setCursor(0, 1);
  lcd.print(&timeinfo, "%H:%M:%S");
}


void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  lcd.init();
  lcd.backlight();
}

void loop() {
  updateLocalTime();  // Обновляем время при необходимости
  printLocalTime();   // Выводим текущее время на экран
  delay(1000);
}
