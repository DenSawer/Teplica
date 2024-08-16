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
