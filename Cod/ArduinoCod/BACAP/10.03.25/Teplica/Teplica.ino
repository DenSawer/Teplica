/*
Проект: Автоматизированная система измерения параметров теплицы
Версия: ESP32_0.3 10.03.25
Создатели: Крючков Д.Д., Радченко Н.О.
*/

#include <DHT.h>                // DHT
#include <Wire.h>               // для шины I2C
#include <LiquidCrystal_I2C.h>  // LCD по I2C
#include <SPI.h>                // SPI (Для Ethernet, SD)
#include <SD.h>                 // SD
#include <ArduinoJson.h>        // JSON для настроек и БД
#include <EthernetENC.h>        // Ethernet модулем ENC28J60
#include <WiFi.h>               // WiFi
#include <time.h>               // time for esp

//--------------------------------ПИНЫ-------------------------------------
#define LDR_PIN 34            // Пин фоторезистора
#define LAMP_PIN 25           // Пин для управления лампой
#define DHT_PIN 15            // Пин для подключения датчика температуры и влажности DHT
#define HEAT_PIN 32           // Пин для управления подогревом (красный светодиод)
#define COOL_PIN 33           // Пин для управления охлаждением (синий светодиод)
#define SOIL_MOISTURE_PIN 35  // Пин для подключения датчика влажности почвы HW-080
#define BIPER_PIN 26          // Бипер для оповещение системы
#define HSPI_MISO 19          // HSPI
#define HSPI_MOSI 23          // HSPI
#define HSPI_CLK 18           // HSPI
#define SD_CS_PIN 4           // HSPI CS для SD
#define ETHERNET_CS_PIN 5     // HSPI CS для Ethernet
#define SIM_RX 16             // UART для SIM800L
#define SIM_TX 17             // UART для SIM800L
#define SDA_PIN 21            // I2C
#define SCL_PIN 22            // I2C

HardwareSerial sim800(1);  // UART для SIM800L

//Подсчет устройств для инициализации
#define allDevice 5  // Все датчики - sd
uint8_t device = 0;

// Структура для хранения настроек
struct {
  String ssid_WiFi = "POCO X6 5G";     // Название сети WiFi
  String password_WiFi = "123456789";  // Пароль сети WiFi
  uint8_t gUTC = 3;                   // Часовой пояс RTC
  bool isCorF = false;                // выбор градус Цельсия(false) или Фаренгейта(true)
} settings;

// Структура для хранения данных теплицы
struct {
  float airTemp;        // температура воздуха
  float soilTemp;       // температура почвы
  uint8_t airHum;       // влажность воздуха
  int16_t soilMois;     // влажность почвы
  uint16_t lightLevel;  // уровень освещенности
  bool isCool;          // холодно
  bool isHeat;          // жарко
  bool isDark;          // темно
} data;

// Структура для хранения инициализированных устройств
struct {
  bool LCD = false;
  bool SD = false;
  bool Ethernet = false;
  bool SIM = false;
  bool airSensor = false;
  bool soilTempSensor = false;
  bool lightSensor = false;
  bool soilMois = false;
} isPresent;

//------------------------Настройки ESP32--------------------------

//---------------RTC---------------
#define NTP_SERVER "pool.ntp.org"   // NTP сервер, для получения времени
time_t lastSyncTime = 0;            // буфферная переменная последнего локальноного времени
const int timeSyncInterval = 3600;  // Интервал синхронизации времени в секундах

//!-----------------------------Дисплеи-------------------------------------!

//---------------LCD---------------
#define LCD_ADDRESS 0x27  // Адресс дисплея
#define LCD_COLUMNS 20    // Кол-во столбцов дисплея
#define LCD_ROWS 4        // Кол-во строк дисплея
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

#define SYM_LEFT_EMPTY 0
#define SYM_CENTER_EMPTY 1
#define SYM_RIGHT_EMPTY 2
#define SYM_GALOCHKA 3
#define SYM_GRADUS_C 4

void loadCustomChars() {
  // Кастомные символы
  uint8_t right_empty[8] = { 31, 1, 1, 1, 1, 1, 1, 31 };
  uint8_t left_empty[8] = { 31, 16, 16, 16, 16, 16, 16, 31 };
  uint8_t center_empty[8] = { 31, 0, 0, 0, 0, 0, 0, 31 };
  uint8_t galochkaChar[8] = { 0, 0, 1, 2, 20, 8, 0, 0 };
  uint8_t gradusCChar[8] = { 28, 20, 28, 3, 4, 4, 4, 3 };
  lcd.createChar(SYM_LEFT_EMPTY, left_empty);
  lcd.createChar(SYM_CENTER_EMPTY, center_empty);
  lcd.createChar(SYM_RIGHT_EMPTY, right_empty);
  lcd.createChar(SYM_GALOCHKA, galochkaChar);
  lcd.createChar(SYM_GRADUS_C, gradusCChar);
}

//!-------------------------Датчики микроклимата---------------------------------!

//---------------DHT--------------
#define DHTTYPE DHT22  // Указываем используемый DHT
DHT dht(DHT_PIN, DHTTYPE);

//!-------------------------Датчики освещённости---------------------------------!

//---------------Фоторезистор--------------

//!-------------------------Датчики влажности почвы---------------------------------!
