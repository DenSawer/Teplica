void loop() {
  updateLocalTime();  // Обновление времени

  readings_DHT();            // Инициализация DHT22
  readings_light();          // Инициализация Датчиков освещённости
  readings_soil_moisture();  // Инициализация Датчиков влажности почвы

  LEDdisplay();
  SerialMonitor();
  //readSQL();
  //ServerRequest();

  delay(1000);
}

void readings_DHT() {
  // Чтение температуры и влажности с DHT22
  microclimate.temperature = dht.readTemperature();  // Чтение температуры
  microclimate.humidity = dht.readHumidity();        // Чтение влажности

  isCool = microclimate.temperature < 28.0;  // холодно если меньше 23
  isHeat = microclimate.temperature > 30.0;  // жарко если больше 24

  // Проверка, удалось ли прочитать данные с датчика DHT22
  if (!isnan(microclimate.humidity) && !isnan(microclimate.temperature)) {
    // Управление подогревом
    if (isCool) digitalWrite(HEAT_PIN, HIGH);  // Включить подогрев
    else digitalWrite(HEAT_PIN, LOW);          // Выключить подогрев
    // Управление охлаждением
    if (isHeat) digitalWrite(COOL_PIN, HIGH);  // Включить охлаждение
    else digitalWrite(COOL_PIN, LOW);          // Выключить охлаждение
  } else {
    Serial.println(F("Ошибка чтения данных с DHT22!"));  // Вывод сообщения об ошибке, если не удалось прочитать данные
    lcd.clear();                                         // Очистка дисплея
    lcd.setCursor(0, 0);                                 // Установка курсора в начало первой строки
    lcd.print("DHT22 Error!");                           // Вывод сообщения об ошибке на дисплей
    delay(500);                                          // Задержка для отображения сообщения
    return;
  }
}

void readings_light() {
  lightLevel = analogRead(LDR_PIN);  // Чтение аналогового значения с пина фоторезистора

  isDark = lightLevel > 1500;  // темно если меньше 1500 // максимальное 4095

  // Включение или выключение лампы на основе показаний фоторезистора
  if (isDark) digitalWrite(LAMP_PIN, HIGH);  // Включить лампу
  else digitalWrite(LAMP_PIN, LOW);          // Выключить лампу
}

void readings_soil_moisture() {
  // Чтение данных с датчика влажности почвы
  soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);  // Чтение аналогового значения с пина датчика влажности почвы
}
