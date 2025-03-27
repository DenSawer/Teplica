void loop() {
  updateLocalTime();  // Обновление времени

  readingsDHT();           // Инициализация DHT22
  readingsLight();         // Инициализация Датчиков освещённости
  readingsSoilMoisture();  // Инициализация Датчиков влажности почвы

  LEDdisplay();
  SerialMonitor();
  saveData();

  delay(1000);
}

void readingsDHT() {
  // Чтение температуры и влажности с DHT22
  data.airTemp = dht.readTemperature();  // Чтение температуры
  data.airHum = dht.readHumidity();      // Чтение влажности

  data.isCool = data.airTemp < 28.0;  // холодно если меньше 23
  data.isHeat = data.airTemp > 30.0;   // жарко если больше 24

  // Проверка, удалось ли прочитать данные с датчика DHT22
  if (!isnan(data.airHum) && !isnan(data.airTemp)) {
    // Управление подогревом
    if (data.isCool) digitalWrite(HEAT_PIN, HIGH);  // Включить подогрев
    else digitalWrite(HEAT_PIN, LOW);          // Выключить подогрев
    // Управление охлаждением
    if (data.isHeat) digitalWrite(COOL_PIN, HIGH);  // Включить охлаждение
    else digitalWrite(COOL_PIN, LOW);          // Выключить охлаждение
  } else {
    Serial.println(F("Ошибка чтения данных с DHT22!"));  // Вывод сообщения об ошибке, если не удалось прочитать данные
    return;
  }
}

void readingsLight() {
  data.lightLevel = analogRead(LDR_PIN);  // Чтение аналогового значения с пина фоторезистора

  data.isDark = data.lightLevel > 1500;  // темно если меньше 1500 // максимальное 4095

  // Включение или выключение лампы на основе показаний фоторезистора
  if (data.isDark) digitalWrite(LAMP_PIN, HIGH);  // Включить лампу
  else digitalWrite(LAMP_PIN, LOW);          // Выключить лампу
}

void readingsSoilMoisture() {
  // Чтение данных с датчика влажности почвы
  data.soilMois = analogRead(SOIL_MOISTURE_PIN);  // Чтение аналогового значения с пина датчика влажности почвы
}
