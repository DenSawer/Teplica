# 1 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino"
/*

Авторы проекта: Крючков Д.Д., Радченко Н.О.

Руководитель проекта: Дрокина Т.М.

*/
# 6 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino"
//-------------------------БИБЛИОТЕКИ----------------------------
# 8 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino" 2
# 9 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino" 2
# 10 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino" 2
# 11 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino" 2
# 12 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino" 2
# 13 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino" 2
# 14 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino" 2


//----------------------------DEFINE----------------------------
# 28 "D:\\Teplica\\ArduinoCod\\Teplica\\Teplica.ino"
//--------------------------ПЕРЕМЕННЫЕ---------------------------

tmElements_t tm;

DHT dht(2 /* пин в который подключен DHT*/, 22 /* здесь указываем тип датчика, сейчас это DHT 22  (AM2302)*/);

LiquidCrystal_I2C lcd(0x27, 16, 2); // создаем объект-экран, передаём используемый адрес и разрешение экрана

GyverTM1637 disp(3 /* пины для 4Digit*/, 4);

uint8_t oldSecond; // переменная для сохранения секунд, используется для задержки с таймера реального времени

uint16_t light; // свет

struct {
  uint8_t humidity; // влажность
  float temperature; // температура
  float heatIndex; // индекс тепла
} microclimate;

bool isCorF = false; // выбор градус Цельсия(false) или Фаренгейта(true)
bool isDark; // темно
bool isCold; // холодно
bool isHot; // жарко
# 1 "D:\\Teplica\\ArduinoCod\\Teplica\\01_setup.ino"
void setup()
{
  Serial.begin(9600);
  dht.begin();

  lcd.init(); // инициализируем экран
  lcd.backlight(); // включаем подсветку

  disp.clear();
  disp.brightness(7); // яркость, 0 - 7 (минимум - максимум)

  Serial.println("LABEL, Time, Minute, Temp, tooCold, tooHot, Light, tooDark, vlaga, index tepla"); // Вывод заголовка столбцов на //последовательный порт
  pinMode(13 /* освещение*/, 0x1); // Настраиваем пин 13 как выход
  pinMode(12 /* нагрев*/, 0x1); // Настраиваем пин 12 как выход
  pinMode(11 /* охлаждение*/, 0x1); // Настраиваем пин 11 как выход
}
# 1 "D:\\Teplica\\ArduinoCod\\Teplica\\02_loop.ino"
void loop() {
  static TimerMs tmr(1000, true); // задержка в 1000мс используемая вместо delay
  if (tmr.tick()) { // запуск задержки

    light = analogRead(A1); // свет

    microclimate.humidity = dht.readHumidity(); // влажность
    microclimate.temperature = dht.readTemperature(isCorF); // температура
    microclimate.heatIndex = dht.convertFtoC(dht.computeHeatIndex(dht.readTemperature(true), microclimate.humidity)); // индекс тепла

    isDark = light > 300; // темно если меньше 300
    isCold = microclimate.temperature < 23.0; // холодно если меньше 23
    isHot = microclimate.temperature > 24.0; // жарко если больше 24

    //Действия в зависимости от значений//
    if (isDark) digitalWrite(13 /* освещение*/, 0x1); // Если слишком темно, включаем светодиод на пине 13
    else digitalWrite(13 /* освещение*/, 0x0); // Иначе выключаем светодиод на пине 13
    if (isCold) digitalWrite(12 /* нагрев*/, 0x1); // Если слишком холодно, включаем светодиод на пине 12
    else digitalWrite(12 /* нагрев*/, 0x0); // Иначе выключаем светодиод на пине 12
    if (isHot) digitalWrite(11 /* охлаждение*/, 0x1); // Если слишком жарко, включаем светодиод на пине 11
    else digitalWrite(11 /* охлаждение*/, 0x0); // Иначе выключаем светодиод на пине 11
  }

  LEDdisplay(); // вывод на дисплей и сериал монитор
}
# 1 "D:\\Teplica\\ArduinoCod\\Teplica\\LEDdisplay.ino"
void LEDdisplay() {
  // кастомные символы
  uint8_t lampChar[] = { 0, 14, 17, 17, 10, 14, 4, 0 };
  uint8_t gradusChar[] = { 12, 18, 18, 12, 0, 0, 0, 0 };

  lcd.createChar(0, gradusChar);
  lcd.createChar(1, lampChar);

  uint8_t errorBanner[] = { _E, _r, _r, _o, _r };
  uint8_t setTimeBanner[] = { _S, _E, _t, _empty, _t, _H, _E, _empty, _t, _1, _n, _n, _E };

  static TimerMs tmr(1000, true); // задержка в 1000мс используемая вместо delay
  if (tmr.tick()) { // запуск задержки
    //вывод парметров и данных теплицы на дисплей//
    if (RTC.read(tm)) { // если датчик времени работает как надо, то выводим
      disp.point(1); // вкл / выкл точку (POINT_ON / POINT_OFF)
      disp.displayClockTwist(tm.Hour, tm.Minute, 35);
    } else {
      if (RTC.chipPresent()) { // если не установлено время
        disp.runningString(setTimeBanner, sizeof(setTimeBanner), 500); // The DS1307 остановлен. Установите время
      } else { // датчик не подключен
        disp.runningString(errorBanner, sizeof(errorBanner), 300); // выводим                                  // DS1307 Ошибка чтения! Проверьте подключение
      }
    }

    lcd.setCursor(0, 0); // устанавливаем курсор в колонку 0, строку 0
    lcd.print(microclimate.temperature);
    lcd.setCursor(4, 0);
    lcd.write(0); // символ градуса
    lcd.print(" ");
    lcd.print(light);
    lcd.write(1); // символ лампочки
    lcd.print(" ");
    lcd.setCursor(11, 0);
    lcd2digits(tm.Hour);
    lcd.write(':');
    lcd2digits(tm.Minute);
    lcd.setCursor(0, 1); // устанавливаем курсор в колонку 0, строку 1
    lcd.print(microclimate.humidity);
    lcd.print("% ");
    lcd2digits(tm.Day);
    lcd.write('/');
    lcd2digits(tm.Month);
    lcd.write('/');
    lcd.print(((tm.Year) - 30) /* offset is from 2000*/); // tmYearToY2k(24), tmYearToCalendar(2024)



    /*lcd2digits(tm.Hour);

    lcd.write(':');

    lcd2digits(tm.Minute);

    lcd.write(':');

    lcd2digits(tm.Second);

    */
# 57 "D:\\Teplica\\ArduinoCod\\Teplica\\LEDdisplay.ino"
    //вывод парметров и данных теплицы на монитор//

    print2digits(tm.Hour); // часы
    Serial.write(":");
    print2digits(tm.Minute); // минуты
    Serial.write(":");
    print2digits(tm.Second); // секунды
    Serial.write(",");
    Serial.print(microclimate.temperature); // температура датчика микроклимата(DHT)
    Serial.write(",");
    Serial.print(isCold); // вкл/выкл охлаждения
    Serial.write(",");
    Serial.print(isHot); // вкл/выкл подогрева
    Serial.write(",");
    Serial.print(light); // значение света
    Serial.write(",");
    Serial.print(isDark); // вкл/выкл освещения
    Serial.write(",");
    Serial.print(microclimate.humidity); // влажность датчика микроклимата(DHT)
    Serial.write(",");
    Serial.print(microclimate.heatIndex); // формула индекса тепла
    Serial.println();
  }
}


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
