void LEDdisplay() {
  // ----------------------Вывод информации на 7seg дисплей-------------------------
  disp.point(POINT_ON);  // вкл / выкл точку (POINT_ON / POINT_OFF)
  disp.displayClockTwist(now.hour(), now.minute(), 35);
  // ----------------------Вывод информации на LCD дисплей--------------------------
  //lcd.clear();                          // Очистка дисплея
  Serial.print("fffffffffffffffffff");
  lcd.setCursor(0, 0);                  // Установка курсора в начало первой строки
  lcd.print("Temp: ");                  // Вывод текста "Temp: "
  lcd.print(microclimate.temperature);  // Вывод температуры
  lcd.print(" C");                      // Вывод единицы измерения (градусы Цельсия)

  lcd.setCursor(0, 1);               // Установка курсора в начало второй строки
  lcd.print("Hum: ");                // Вывод текста "Hum: "
  lcd.print(microclimate.humidity);  // Вывод влажности
  lcd.print("%");                    // Вывод единицы измерения (проценты)

  lcd.setCursor(9, 1);       // Установка курсора для вывода времени
  lcd2digits(now.hour());    // Вывод часов
  lcd.print(':');            
  lcd2digits(now.minute());  // Вывод минут
  lcd.print(':');            
  lcd2digits(now.second());  // Вывод минут
}

void lcd2digits(uint8_t number) {
  if (number >= 0 && number < 10) {
    lcd.write('0');
  }
  lcd.print(number);
}