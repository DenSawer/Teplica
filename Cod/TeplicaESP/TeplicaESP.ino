/*
Авторы проекта: Крючков Д.Д., Радченко Н.О.
Руководитель проекта: Дрокина Т.М.
*/

//-------------------------БИБЛИОТЕКИ----------------------------
#include <DHT.h>                // датчик микроклимата DHT22
#include <Wire.h>               // подключаем библиотеку Wire
#include <LiquidCrystal_I2C.h>  // подключаем библиотеку ЖКИ
#include <TimeLib.h>            // для датчика реального времени
#include <DS1307RTC.h>          // датчик реального времени DS1307
#include <TimerMs.h>            // библиотека задержек
#include <GyverTM1637.h>        // для 4 digital display


//----------------------------DEFINE----------------------------
#define lighting 13  // освещение
#define heating 12   // нагрев
#define cooling 11   // охлаждение

#define DHTPIN 2       // пин в который подключен DHT
#define DHTTYPE DHT22  // здесь указываем тип датчика, сейчас это DHT 22  (AM2302)

#define CLK 3  // пины для 4Digit
#define DIO 4


//--------------------------ПЕРЕМЕННЫЕ---------------------------

tmElements_t tm;

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);  // создаем объект-экран, передаём используемый адрес и разрешение экрана

GyverTM1637 disp(CLK, DIO);

uint8_t oldSecond;  // переменная для сохранения секунд, используется для задержки с таймера реального времени

uint16_t light;  // свет

struct {
  uint8_t humidity;   // влажность
  float temperature;  // температура
  float heatIndex;    // индекс тепла
} microclimate;

bool isCorF = false; // выбор градус Цельсия(false) или Фаренгейта(true)
bool isDark;  // темно
bool isCold;  // холодно
bool isHot;   // жарко
