void LEDdisplay() {
  // ----------------------Вывод информации на LCD дисплей--------------------------
  //lcd.clear();                          // Очистка дисплея
  lcd.setCursor(0, 0);  // Установка курсора в начало первой строки
  struct tm timeinfo;   // Для отображения времени
  if (!getLocalTime(&timeinfo)) {
    lcd.print("no time");
  } else {
    lcd.print(&timeinfo, "%Y/%m/%d ");
    lcd.print(&timeinfo, "%H:%M:%S");
  }

  lcd.setCursor(0, 1);         // Установка курсора в начало второй строки
  lcd.print("T:");             // Вывод текста "Temp: "
  lcd.print(data.airTemp, 1);  // Вывод температуры
  lcd.write(SYM_GRADUS_C);     // Вывод единицы измерения (градусы Цельсия)
  lcd.print(" H:");            // Вывод текста "Hum: "
  lcd.print(data.airHum);      // Вывод влажности
  lcd.print("%");              // Вывод единицы измерения (проценты)
  lcd.print(" L:");            // Вывод текста "Hum: "
  lcd.print(data.lightLevel);  // Вывод влажности
  lcd.print("   ");  
}

void lcd2digits(uint8_t number) {
  if (number >= 0 && number < 10) {
    lcd.write('0');
  }
  lcd.print(number);
}