void initLCD() {
  // Проверка наличия и автоматическое определение LCD дисплея
  if (!Wire.requestFrom(LCD_ADDRESS, 1)) {
    isPresent.LCD = false;
    showLoadingProgressBar("Display", false);
    delay(1000);
    return;
  }

  loadCustomChars();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(200);
  isPresent.LCD = true;
  showLoadingProgressBar("Display", true);
}

void displayMonitor() {
  static unsigned long lastDisplayUpdate = 0;  // Время последнего обновления
  unsigned long currentMillis = millis();      // Текущее время

  if (currentMillis - lastDisplayUpdate < 1000) return;  // Обновляем раз в секунду
  lastDisplayUpdate = currentMillis;

  // Получаем время
  char timeStr[6] = "--:--";
  bool timeAvailable = false;
  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);  // Формат ЧЧ:ММ
    timeAvailable = true;
  }

  char line[21];  // Буфер для строки

  // --- Строка 1: Температура воздуха и время ---
  if (timeAvailable) {
    snprintf(line, sizeof(line), "CO2:  %uppm  %s", data.CO2ppm, timeStr);
  } else {
    snprintf(line, sizeof(line), "CO2:  %uppm  --:--", data.CO2ppm);
  }
  padAndPrint(0, 0, line);

  // --- Строка 2: Температура почвы и влажность воздуха ---
  snprintf(line, sizeof(line), "Air:  %.1f\xDF\C  %u%%", data.airTemp, data.airHum);
  padAndPrint(0, 1, line);

  // --- Строка 3: Влажность почвы и освещённость ---
  snprintf(line, sizeof(line), "Soil: %.1f\xDF\C  %d%%", data.soilTemp, data.soilMois);
  padAndPrint(0, 2, line);

  // --- Строка 4: CO2 уровень ---
  snprintf(line, sizeof(line), "Light:%uLx", data.lightLevel);
  padAndPrint(0, 3, line);

  /*// --- Строка 1: Температура воздуха и время ---
  if (timeAvailable) {
    snprintf(line, sizeof(line), "Air:%.1fC   Time:%s", data.airTemp, timeStr);
  } else {
    snprintf(line, sizeof(line), "Air:%.1fC   Time:--:--", data.airTemp);
  }
  padAndPrint(0, 0, line);

  // --- Строка 2: Температура почвы и влажность воздуха ---
  snprintf(line, sizeof(line), "Soil:%.1fC  RH:%u%%", data.soilTemp, data.airHum);
  padAndPrint(0, 1, line);

  // --- Строка 3: Влажность почвы и освещённость ---
  snprintf(line, sizeof(line), "SoilM:%d%% Light:%ulx", data.soilMois, data.lightLevel);
  padAndPrint(0, 2, line);

  // --- Строка 4: CO2 уровень ---
  snprintf(line, sizeof(line), "CO2: %u ppm", data.CO2ppm);
  padAndPrint(0, 3, line);*/
}


// 📦 Вспомогательная функция — дополняет строку пробелами до 20 символов
void padAndPrint(uint8_t col, uint8_t row, const char* text) {
  char buffer[21];
  size_t len = strlen(text);

  // Копируем и заполняем пробелами при необходимости
  strncpy(buffer, text, sizeof(buffer));
  if (len < 20) {
    memset(buffer + len, ' ', 20 - len);
  }

  buffer[20] = '\0';  // Гарантируем завершение строки
  lcd.setCursor(col, row);
  lcd.print(buffer);  // Печатаем всю строку сразу
}



/*struct tm timeinfo;   // Для отображения времени
  if (!getLocalTime(&timeinfo)) {
    lcd.print("no time");
  } else {
    lcd.print(&timeinfo, "%d/%m/%Y ");
    lcd.print(&timeinfo, "%H:%M:%S");
  }*/