/*
Проект: Автоматизированная система измерения параметров теплицы
Версия: ESP32_0.3 07.04.25
Создатели: Крючков Д.Д., Радченко Н.О.
*/

#include <Wire.h>               // для шины I2C
#include <Adafruit_Sensor.h>    // общая библиотека для Adafruit
#include <BH1750.h>             // BH1750 (lightSensor)
#include <Adafruit_BME280.h>    // BME280 (airSensor)
#include <Adafruit_SHT4x.h>     // SHT4x (airSensor)
#include <Adafruit_SHT31.h>     // SHT31 (airSensor)
#include <SHT2x.h>              // SHT2x (airSensor)
#include <DHT.h>                // DHT (airSensor)
#include <OneWire.h>            // Библиотека считывания показаний с DS18B20 (soilTempSensor)
#include <DallasTemperature.h>  // Преобразование показаний DS18B20 и выдача в нормальном формате (soilTempSensor)
#include <SensirionI2cScd4x.h>  // SCD40 or SCD41 (CO2Sensor)
#include <Adafruit_SCD30.h>     // SCD31 (CO2Sensor)
#include <LiquidCrystal_I2C.h>  // LCD по I2C
#include <SPI.h>                // SPI (Для Ethernet, SD)
#include <SD.h>                 // SD
#include <ArduinoJson.h>        // JSON для настроек и БД
#include <WiFi.h>               // WiFi
#include <time.h>               // time for esp
#include <WebServer.h>          // для раздачи веб-интерфейса
#include <DNSServer.h>          // для открытия веб-интрефейса автоматом(принудительно)
#include <UIPEthernet.h>        // Ethernet
#include <HTTPClient.h>         // для создания веб-интерфейса

//--------------------------------ПИНЫ-------------------------------------
#define LDR_PIN 34            // Пин фоторезистора
#define LAMP_PIN 25           // Пин для управления лампой
#define DHT_PIN 15            // Пин для подключения датчика температуры и влажности DHT
#define HEAT_PIN 32           // Пин для управления подогревом (красный светодиод)
#define COOL_PIN 33           // Пин для управления охлаждением (синий светодиод)
#define SOIL_MOISTURE_PIN 35  // Пин для подключения датчика влажности почвы HW-080
#define SOIL_TEMP_PIN 14      // Пин для подключения датчика температуры почвы ds18b20
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
EthernetClient client;     // для Ethernet

// Структура для хранения настроек
struct {
  // Данные для подключения к заданной Wi-Fi сети
  String ssid_WiFi = "POCO X6 5G";     // Название сети WiFi
  String password_WiFi = "123456789";  // Пароль сети WiFi
  // Данные для создания точки доступа
  String ssid_AP = "Teplica";  // Название сети WiFi точки доступа
  uint8_t gUTC = 3;            // Часовой пояс RTC
  bool isCorF = false;         // выбор градус Цельсия(false) или Фаренгейта(true)
} settings;

// Структура для хранения данных теплицы
struct {
  float airTemp = 0.0;      // температура воздуха °C
  float soilTemp = 0.0;     // температура почвы °C
  uint8_t airHum = 0;       // влажность воздуха %
  int8_t soilMois = 0;      // влажность почвы %
  uint16_t lightLevel = 0;  // уровень освещенности Лк
  uint16_t CO2ppm = 0;      // уровень освещенности ppm
  bool isCool = false;      // холодно
  bool isHeat = false;      // жарко
  bool isDark = false;      // темно
} data;

// Структура для хранения инициализированных устройств
struct {
  bool LCD = false;
  bool SD = false;
  bool Ethernet = false;
  bool SIM = false;
  bool airSensor = false;
  bool CO2Sensor = false;
  bool soilTempSensor = false;
  bool lightSensor = false;
  bool soilMoisSensor = false;
} isPresent;

//------------------------Настройки ESP32--------------------------

//---------------RTC---------------
#define NTP_SERVER "pool.ntp.org"        // NTP сервер, для получения времени
time_t lastSyncTime = 0;                 // буфферная переменная последнего локальноного времени
const uint16_t timeSyncInterval = 3600;  // Интервал синхронизации времени в секундах
//---------------AP----------------
// Создаем веб-сервер на порту 80
WebServer server(80);
DNSServer dnsServer;  // Для Captive Portal

//!------------------------------SIM---------------------------------!

const char *defaultAPN = "";
const char *defaultUser = "";
const char *defaultPass = "";
// лист оператор с их апн, юзером и паролем
const char *apnList[][4] = {
  { "25099", "internet.beeline.ru", "", "" },
  { "25001", "internet.mts.ru", "mts", "mts" },
  { "25002", "internet", "gdata", "gdata" },
  { "25020", "internet.tele2.ru", "tele2", "tele2" },
  { "25026", "internet.yota", "", "" },
  { "25096", "Inter96", "Inter96", "Inter96" }
};
bool InternetFromSIM = false;

//!-----------------------------Дисплей-------------------------------------!

#define LCD_ADDRESS 0x27  // Адресс дисплея
#define LCD_COLUMNS 20    // Кол-во столбцов дисплея
#define LCD_ROWS 4        // Кол-во строк дисплея
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

#define SYM_GALOCHKA 3
#define SYM_GRADUS_C 4

void loadCustomChars() {
  // Кастомные символы
  uint8_t galochkaChar[8] = { 0, 0, 1, 2, 20, 8, 0, 0 };
  uint8_t gradusCChar[8] = { 28, 20, 28, 3, 4, 4, 4, 3 };
  lcd.createChar(SYM_GALOCHKA, galochkaChar);
  lcd.createChar(SYM_GRADUS_C, gradusCChar);
}

//!-----------------Датчики температуры и влажности воздуха-------------------!

// Объекты датчиков
DHT dht(DHT_PIN, DHT22);
Adafruit_BME280 bme;
Adafruit_SHT4x sht4;
Adafruit_SHT31 sht3;
SHT2x sht2;

bool bme_present = false;
bool sht4_present = false;
bool sht3_present = false;
bool sht2_present = false;

//!-------------------------Датчики уровня СО2 в воздухе---------------------------------!

SensirionI2cScd4x scd4x;
Adafruit_SCD30 scd30;
bool isSCD30 = false;
bool isSCD4x = false;

//!-------------------------Датчики освещённости---------------------------------!

BH1750 lightMeter;
bool bh1750_present = false;

//!-------------------------Датчики влажности почвы---------------------------------!

//аналоговый емкостной v1.2, сразу считываем поэтому тут голяк

//!-------------------------Датчики температуры почвы---------------------------------!

OneWire oneWire(SOIL_TEMP_PIN);
DallasTemperature sensors(&oneWire);
bool ds18b20_present = false;
uint8_t sensorCount = 0;

//------------Other-------------
void showLoadingProgressBar(const char *str, bool yes_no = false);
