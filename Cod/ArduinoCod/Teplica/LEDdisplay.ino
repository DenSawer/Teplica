void LEDdisplay() {
  // ----------------------Вывод информации на LCD дисплей--------------------------
  //lcd.clear();                          // Очистка дисплея
  lcd.setCursor(0, 0);                     // Установка курсора в начало первой строки
  lcd.print("T:");                         // Вывод текста "Temp: "
  lcd.print(microclimate.temperature, 1);  // Вывод температуры
  lcd.print(char(1));                      // Вывод единицы измерения (градусы Цельсия)
  lcd.print(" H:");                        // Вывод текста "Hum: "
  lcd.print(microclimate.humidity);        // Вывод влажности
  lcd.print("%");                          // Вывод единицы измерения (проценты)

  lcd.setCursor(0, 1);    // Установка курсора в начало второй строки
  lcd.print("L:");        // Вывод текста "Hum: "
  lcd.print(lightLevel);  // Вывод влажности
  lcd.print(" ");
  lcd.setCursor(7, 1);
  lcd2digits(now.hour());  // Вывод часов
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