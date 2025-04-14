/*
Проект: Автоматизированная система измерения параметров теплицы
Версия: ESP32_0.5 14.04.25
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
#include <EEPROM.h>
#include <PCF8574.h>

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
#define KNOPKA_PIN            // Кнопка включения дисплея

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
} settings;

// Структура для хранения данных теплицы
struct {
  float airTemp = 0.0;      // температура воздуха °C
  float soilTemp = 0.0;     // температура почвы °C
  uint8_t airHum = 0;       // влажность воздуха %
  int8_t soilMois = 0;      // влажность почвы %
  uint16_t lightLevel = 0;  // уровень освещенности Лк
  uint16_t CO2ppm = 0;      // уровень освещенности ppm
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

// === 🔧 Конфигурация климат-контроля ===
// Дневные и ночные пороги температуры воздуха
float T_air_day_min = 17.0;    // минимальная температура воздуха днём
float T_air_day_max = 25.0;    // максимальная температура воздуха днём
float T_air_night_min = 15.0;  // минимальная температура воздуха ночью
float T_air_night_max = 22.0;  // максимальная температура воздуха ночью
float T_air_cool_off = 21.0;   // температура выключения вентиляции

// Температура почвы
float T_soil_min = 18.0;  // минимальная температура почвы
float T_soil_off = 20.0;  // температура, при которой выключаем обогрев

// Влажность воздуха
uint8_t H_air_min = 45;  // нижний порог включения увлажнителя
uint8_t H_air_max = 65;  // максимальный допустимый уровень влажности
uint8_t H_air_ok = 55;   // порог отключения увлажнителя

// Влажность почвы
int8_t M_soil_min = 85;  // порог включения полива
int8_t M_soil_max = 90;  // порог отключения полива

// CO₂ концентрация
uint16_t CO2_min = 200;  // нижний порог включения вентиляции
uint16_t CO2_ok = 350;   // порог отключения вентиляции

// Освещение
uint16_t light_min = 3000;      // минимальный уровень освещённости
uint8_t light_start_hour = 17;  // с какого часа включать досветку
uint8_t light_end_hour = 20;    // до какого часа включать досветку

// === 🔌 Индексы реле через PCF8574 ===
PCF8574 pcf(0x20);  // Устройство PCF8574 (I2C, 0x20 — адрес по умолчанию)
#define RELAY_HEATER 0
#define RELAY_FAN 1
#define RELAY_HUMIDIFIER 2
#define RELAY_PUMP 3
#define RELAY_LIGHT 4
#define RELAY_VENT_CO2 5

bool relayState[8] = { false };

//------------------------Настройки ESP32--------------------------

//---------------RTC---------------
#define NTP_SERVER "pool.ntp.org"        // NTP сервер, для получения времени
time_t lastSyncTime = 0;                 // буфферная переменная последнего локальноного времени
const uint16_t timeSyncInterval = 3600;  // Интервал синхронизации времени в секундах
//---------------AP----------------
// Создаем веб-сервер на порту 80
WebServer server(80);
DNSServer dnsServer;  // Для Captive Portal

bool isConnect = false;
#define EEPROM_FLAG_ADDR 0  // 1 байт: есть ли сохранение
#define EEPROM_MAC_ADDR 1   // 6 байт: MAC
#define EEPROM_IP_ADDR 7    // 4 байта: IP

bool knopka = true;

//!------------------------------SIM---------------------------------!

const char *defaultAPN = "mcs";
const char *defaultUser = "";
const char *defaultPass = "";
// лист оператор с их апн, юзером и паролем
const char *apnList[][4] = {
  { "25099", "internet.beeline.ru", "", "" },
  { "25001", "internet.mts.ru", "mts", "mts" },
  { "25002", "internet", "gdata", "gdata" },
  { "25020", "internet.tele2.ru", "tele2", "tele2" },
  { "25026", "internet.yota", "", "" },
  { "25096", "Inter96", "Inter96", "Inter96" },
  { "25098", "mcs", "", "" }
};
bool InternetFromSIM = false;

//!-----------------------------Дисплей-------------------------------------!

#define LCD_ADDRESS 0x27  // Адресс дисплея
#define LCD_COLUMNS 20    // Кол-во столбцов дисплея
#define LCD_ROWS 4        // Кол-во строк дисплея
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

#define SYM_GALOCHKA 3
#define SYM_CELSIUS 4
#define SYM_GRADUS_C 5

void loadCustomChars() {
  // Кастомные символы
  uint8_t galochkaChar[8] = { 0, 0, 1, 2, 20, 8, 0, 0 };
  uint8_t celsiusChar[8] = { 28, 20, 28, 3, 4, 4, 4, 3 };
  uint8_t gradusCChar[8] = { 0x1E, 0x12, 0x12, 0x1E, 0x00, 0x00, 0x00, 0x00 };
  lcd.createChar(SYM_GALOCHKA, galochkaChar);
  lcd.createChar(SYM_CELSIUS, celsiusChar);
  lcd.createChar(SYM_GRADUS_C, gradusCChar);
}

//!-----------------Датчики температуры и влажности воздуха-------------------!

// Объекты датчиков
DHT dht(DHT_PIN, DHT22);
Adafruit_BME280 bme;
Adafruit_SHT4x sht4;
Adafruit_SHT31 sht3;
SHT2x sht2;

//!-------------------------Датчики уровня СО2 в воздухе---------------------------------!

SensirionI2cScd4x scd4x;
Adafruit_SCD30 scd30;

//!-------------------------Датчики освещённости---------------------------------!

BH1750 lightMeter;

//!-------------------------Датчики влажности почвы---------------------------------!

//аналоговый емкостной v1.2, сразу считываем поэтому тут голяк

//!-------------------------Датчики температуры почвы---------------------------------!

OneWire oneWire(SOIL_TEMP_PIN);
DallasTemperature sensors(&oneWire);
bool ds18b20_present = false;
uint8_t sensorCount = 0;

//------------Other-------------
void showLoadingProgressBar(const char *str, bool yes_no = false);
