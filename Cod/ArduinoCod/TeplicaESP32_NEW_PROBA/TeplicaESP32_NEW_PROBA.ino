/*
Проект: Автотеплица
Версия: ESP32_0.1
Создатели: Крючков Д.Д., Радченко Н.О.
Руководители: Дрокина Т.М., Захлестов А.М.
*/

#include <DHT.h>                // DHT
#include <Wire.h>               // для шины I2C
#include "LiquidCrystal_I2C.h"  // LCD по I2C
#include <GyverTM1637.h>        // для 4 digital display
#include <RTClib.h>             // RTC (часы реального времени)
#include <SPI.h>                // SPI (Для Ethernet, SD)
#include <SD.h>                 // SD
#include <EthernetENC.h>        // Ethernet модулем ENC28J60


//--------------------------------ПИНЫ-------------------------------------
#define LDR_PIN 34            // Пин фоторезистора
#define LAMP_PIN 25           // Пин для управления лампой
#define DHT_PIN 4             // Пин для подключения датчика температуры и влажности DHT
#define HEAT_PIN 32           // Пин для управления подогревом (красный светодиод)
#define COOL_PIN 33           // Пин для управления охлаждением (синий светодиод)
#define SOIL_MOISTURE_PIN 35  // Пин для подключения датчика влажности почвы HW-080
#define ETHERNET_CS_PIN 5     // Пин CS для Ethernet
#define HSPI_MISO 12          // Определение пинов для HSPI
#define HSPI_MOSI 13
#define HSPI_CLK 14
#define HSPI_CS 15

// -------------------------DHT (датчик микроклимата)---------------------------------
#define DHTTYPE DHT22  // Указываем используемый DHT
DHT dht(DHT_PIN, DHTTYPE);

// -----------------------------Дисплеи-------------------------------------
//----------------7seg disp-----------
GyverTM1637 disp(CLK, DIO);
//------------------LCD---------------
uint8_t lcdColumn = 16;
uint8_t lcdRow = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumn, lcdRow);

// ------------------------RTC (часы реального времени)--------------------------
RTC_DS1307 rtc;
DateTime now;  // переменая времени сейчас

//----------------------------------SD-----------------------------------
SPIClass hspi(HSPI);  // Объект HSPI

// ------------------------Ethernet ENC28J60---------------------
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

//------------------------------ПЕРЕМЕННЫЕ----------------------------

uint16_t ldrValue;  // свет

struct {
  uint8_t humidity;   // влажность
  float temperature;  // температура
  float heatIndex;    // индекс тепла
} microclimate;

uint16_t soilMoistureValue;

bool isCorF = false;  // выбор градус Цельсия(false) или Фаренгейта(true)
bool isDark;          // темно
bool isCool;          // холодно
bool isHeat;          // жарко
