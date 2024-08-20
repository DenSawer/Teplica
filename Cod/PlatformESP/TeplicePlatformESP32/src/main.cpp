#include <Arduino.h>

/*
Авторы проекта: Крючков Д.Д., Радченко Н.О.
Руководитель проекта: Дрокина Т.М.
*/

//-------------------------БИБЛИОТЕКИ----------------------------
#include "DHT.h"                // датчик микроклимата DHT22
#include "Wire.h"               // подключаем библиотеку Wire
#include "LiquidCrystal_I2C.h"  // подключаем библиотеку ЖКИ
#include "TimeLib.h"            // для датчика реального времени
#include "DS1307RTC.h"          // датчик реального времени DS1307
#include "TimerMs.h"            // библиотека задержек
#include "GyverTM1637.h"        // для 4 digital display


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

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);  // создаем объект-экран, передаём используемый адрес и разрешение экрана

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


void lcd2digits(uint8_t number) {
  if (number >= 0 && number < 10) {
    lcd.write('0');
  }
  lcd.print(number);
}
void print2digits(uint8_t number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}



void LEDdisplay() {
  // кастомные символы
  uint8_t lampChar[] = { 0, 14, 17, 17, 10, 14, 4, 0 };
  uint8_t gradusChar[] = { 12, 18, 18, 12, 0, 0, 0, 0 };

  lcd.createChar(0, gradusChar);
  lcd.createChar(1, lampChar);

  uint8_t errorBanner[] = { _E, _r, _r, _o, _r };
  uint8_t setTimeBanner[] = { _S, _E, _t, _empty, _t, _H, _E, _empty, _t, _1, _n, _n, _E };

  static TimerMs tmr(1000, true);  // задержка в 1000мс используемая вместо delay
  if (tmr.tick()) {                // запуск задержки
    //вывод парметров и данных теплицы на дисплей//
    if (RTC.read(tm)) {      // если датчик времени работает как надо, то выводим
      disp.point(POINT_ON);  // вкл / выкл точку (POINT_ON / POINT_OFF)
      disp.displayClockTwist(tm.Hour, tm.Minute, 35);
    } else {
      if (RTC.chipPresent()) {                                          // если не установлено время
        disp.runningString(setTimeBanner, sizeof(setTimeBanner), 500);  // The DS1307 остановлен. Установите время
      } else {                                                          // датчик не подключен
        disp.runningString(errorBanner, sizeof(errorBanner), 300);      // выводим                                  // DS1307 Ошибка чтения! Проверьте подключение
      }
    }

    lcd.setCursor(0, 0);  // устанавливаем курсор в колонку 0, строку 0
    lcd.print(microclimate.temperature);
    lcd.setCursor(4, 0);
    lcd.write(0);  // символ градуса
    lcd.print(" ");
    lcd.print(light);
    lcd.write(1); // символ лампочки
    lcd.print(" ");
    lcd.setCursor(11, 0);
    lcd2digits(tm.Hour);
    lcd.write(':');
    lcd2digits(tm.Minute);
    lcd.setCursor(0, 1);  // устанавливаем курсор в колонку 0, строку 1
    lcd.print(microclimate.humidity);
    lcd.print("% ");
    lcd2digits(tm.Day);
    lcd.write('/');
    lcd2digits(tm.Month);
    lcd.write('/');
    lcd.print(tmYearToY2k(tm.Year));  // tmYearToY2k(24), tmYearToCalendar(2024)



    /*lcd2digits(tm.Hour);
    lcd.write(':');
    lcd2digits(tm.Minute);
    lcd.write(':');
    lcd2digits(tm.Second);
    */


    //вывод парметров и данных теплицы на монитор//

    print2digits(tm.Hour);  // часы
    Serial.write(":");
    print2digits(tm.Minute);  // минуты
    Serial.write(":");
    print2digits(tm.Second);  // секунды
    Serial.write(",");
    Serial.print(microclimate.temperature);  // температура датчика микроклимата(DHT)
    Serial.write(",");
    Serial.print(isCold);  // вкл/выкл охлаждения
    Serial.write(",");
    Serial.print(isHot);  // вкл/выкл подогрева
    Serial.write(",");
    Serial.print(light);  // значение света
    Serial.write(",");
    Serial.print(isDark);  // вкл/выкл освещения
    Serial.write(",");
    Serial.print(microclimate.humidity);  // влажность датчика микроклимата(DHT)
    Serial.write(",");
    Serial.print(microclimate.heatIndex);  // формула индекса тепла
    Serial.println();
  }
}




void setup()
{
  Serial.begin(9600);
  dht.begin();

  //lcd.init(); // инициализируем экран
  lcd.backlight(); // включаем подсветку

  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)

  Serial.println("LABEL, Time, Minute, Temp, tooCold, tooHot, Light, tooDark, vlaga, index tepla");  // Вывод заголовка столбцов на //последовательный порт
  pinMode(lighting, OUTPUT); // Настраиваем пин 13 как выход
  pinMode(heating, OUTPUT); // Настраиваем пин 12 как выход
  pinMode(cooling, OUTPUT); // Настраиваем пин 11 как выход
}


void loop() {
  static TimerMs tmr(1000, true);  // задержка в 1000мс используемая вместо delay
  if (tmr.tick()) {                // запуск задержки

    light = analogRead(2);  // свет

    microclimate.humidity = dht.readHumidity();                                                                        // влажность
    microclimate.temperature = dht.readTemperature(isCorF);                                                            // температура
    microclimate.heatIndex = dht.convertFtoC(dht.computeHeatIndex(dht.readTemperature(true), microclimate.humidity));  // индекс тепла

    isDark = light > 300;                      // темно если меньше 300
    isCold = microclimate.temperature < 23.0;  // холодно если меньше 23
    isHot = microclimate.temperature > 24.0;   // жарко если больше 24

    //Действия в зависимости от значений//
    if (isDark) digitalWrite(lighting, HIGH);  // Если слишком темно, включаем светодиод на пине 13
    else digitalWrite(lighting, LOW);          // Иначе выключаем светодиод на пине 13
    if (isCold) digitalWrite(heating, HIGH);   // Если слишком холодно, включаем светодиод на пине 12
    else digitalWrite(heating, LOW);           // Иначе выключаем светодиод на пине 12
    if (isHot) digitalWrite(cooling, HIGH);    // Если слишком жарко, включаем светодиод на пине 11
    else digitalWrite(cooling, LOW);           // Иначе выключаем светодиод на пине 11
  }

  LEDdisplay();  // вывод на дисплей и сериал монитор
}

