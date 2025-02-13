/*
Проект: Автоматизированная система измерения параметров теплицы
Версия: ESP32_0.2 29.01.25
Создатели: Крючков Д.Д., Радченко Н.О.
Руководители: Дрокина Т.М., Захлестов А.М.
*/

#include <DHT.h>                // DHT
#include <Wire.h>               // для шины I2C
#include "LiquidCrystal_I2C.h"  // LCD по I2C
#include <RTClib.h>             // RTC (часы реального времени)
#include <SPI.h>                // SPI (Для Ethernet, SD)
#include <SD.h>                 // SD
#include <sqlite3.h>            // SQL (База данных)
#include <EthernetENC.h>        // Ethernet модулем ENC28J60


//--------------------------------ПИНЫ-------------------------------------
#define LDR_PIN 34            // Пин фоторезистора
#define LAMP_PIN 25           // Пин для управления лампой
#define DHT_PIN 15            // Пин для подключения датчика температуры и влажности DHT
#define HEAT_PIN 32           // Пин для управления подогревом (красный светодиод)
#define COOL_PIN 33           // Пин для управления охлаждением (синий светодиод)
#define SOIL_MOISTURE_PIN 35  // Пин для подключения датчика влажности почвы HW-080
#define ETHERNET_CS_PIN 5     // Пин CS для Ethernet
#define HSPI_MISO 19          // HSPI для SD
#define HSPI_MOSI 23          // для SD
#define HSPI_CLK 18           // для SD
#define HSPI_CS 4             // для SD
#define BIPER_PIN 26          // Бипер для оповещение системы
#define MODEM_RX 16           // Пины подключения к SIM800L
#define MODEM_TX 17

HardwareSerial sim800(1);

//Подсчет устройств для инициализации
#define allDevice 6  // Все датчики - sd
uint8_t device = 0;

//!-----------------------------Дисплеи-------------------------------------!

//---------------LCD---------------
#define LCD_ADDRESS 0x27
#define LCD_COLUMN 16
#define LCD_ROW 2
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMN, LCD_ROW);  // Временная инициализация для проверки

//!------------------------RTC (часы реального времени)--------------------------!

RTC_DS1307 rtc;
DateTime now;  // переменая времени сейчас

//!-------------------------------SD---------------------------------------!

SPIClass hspi(HSPI);  // Объект HSPI

//------------------------------SQL---------------------------------------

sqlite3 *db;

//!------------------------Ethernet ENC28J60---------------------------!

uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC адрес устройства
IPAddress ip(192, 168, 1, 28);                           // Статический IP адрес
IPAddress dnsServer(192, 168, 1, 1);                     // IP адрес DNS сервера
IPAddress gw(192, 168, 1, 1);                            // IP адрес шлюза
IPAddress sn(255, 255, 255, 0);                          // Маска подсети
char server[] = "httpbin.org";                           // Сервер для подключения по HTTP
EthernetClient client;                                   // Создание клиента для Ethernet

uint32_t beginMicros, endMicros;  // Переменные для измерения скорости передачи данных
uint32_t byteCount = 0;           // Счетчик принятых байт
bool printWebData = true;         // Флаг для вывода данных в Serial Monitor

//!-------------------------Датчики микроклимата---------------------------------!

struct {
  uint8_t humidity;   // влажность
  float temperature;  // температура
  float heatIndex;    // индекс тепла
} microclimate;

bool isCool;          // холодно
bool isHeat;          // жарко
bool isCorF = false;  // выбор градус Цельсия(false) или Фаренгейта(true)

//---------------DHT--------------
#define DHTTYPE DHT22  // Указываем используемый DHT
DHT dht(DHT_PIN, DHTTYPE);

//!-------------------------Датчики освещённости---------------------------------!

bool isDark;  // темно

//---------------Фоторезистор--------------
uint16_t lightLevel;  // уровень освещенности

//!-------------------------Датчики влажности почвы---------------------------------!

uint16_t soilMoistureValue;  // уровень влажности
