/*
Проект: Автотеплица
Версия: ESP32_0.1
Создатели: Крючков Д.Д., Радченко Н.О.
Руководители: Дрокина Т.М., Захлестов А.М.
*/

#include <DHT.h>                // Библиотека для работы с датчиком DHT
#include <Wire.h>               // Библиотека для работы с шиной I2C
#include "LiquidCrystal_I2C.h"  // Библиотека для работы с LCD по I2C
#include <RTClib.h>             // Библиотека для работы с RTC (часы реального времени)
#include <SPI.h>                // Библиотека для работы с SPI (Для Ethernet)
#include <EthernetENC.h>        // Библиотека для работы с Ethernet модулем ENC28J60

//--------------------------------ПИНЫ-------------------------------------
#define LDR_PIN 34            // Пин фоторезистора
#define LAMP_PIN 25           // Пин для управления лампой
#define DHT_PIN 4             // Пин для подключения датчика температуры и влажности DHT
#define HEAT_PIN 32            // Пин для управления подогревом (красный светодиод)
#define COOL_PIN 33           // Пин для управления охлаждением (синий светодиод)
#define SOIL_MOISTURE_PIN 35  // Пин для подключения датчика влажности почвы HW-080
#define ETHERNET_CS_PIN 5     // Пин CS для Ethernet

// -------------------------DHT (датчик микроклимата)---------------------------------
#define DHTTYPE DHT22  // Указываем используемый DHT
DHT dht(DHT_PIN, DHTTYPE);

// -----------------------------LCD (дисплей)-------------------------------------
uint8_t lcdColumn = 16;
uint8_t lcdRow = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumn, lcdRow);

// ------------------------RTC (часы реального времени)--------------------------
RTC_DS1307 rtc;

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
bool isHeat;           // жарко

DateTime now;