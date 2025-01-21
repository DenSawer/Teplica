void loop() {
  lcd.setCursor(0, 0);
  lcd.print("str");
  lcd.print("Temp: ");                  // Вывод текста "Temp: "
  lcd.print(microclimate.temperature);  // Вывод температуры
  // Чтение показаний с фоторезистора
  lightLevel = analogRead(LDR_PIN);  // Чтение аналогового значения с пина фоторезистора

  // Чтение данных с датчика влажности почвы
  soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);  // Чтение аналогового значения с пина датчика влажности почвы

  // Чтение температуры и влажности с DHT22
  microclimate.temperature = dht.readTemperature();  // Чтение температуры
  microclimate.humidity = dht.readHumidity();        // Чтение влажности

  // Получение текущего времени с RTC
  now = rtc.now();

  lcd.setCursor(0, 1);               // Установка курсора в начало второй строки
  lcd.print("Hum: ");                // Вывод текста "Hum: "
  lcd.print(microclimate.humidity);  // Вывод влажности

  isDark = lightLevel > 1500;                // темно если меньше 1500 // максимальное 4096
  isCool = microclimate.temperature < 28.0;  // холодно если меньше 23
  isHeat = microclimate.temperature > 30.0;  // жарко если больше 24

  //------------Действия в зависимости от значений-----------------

  // Включение или выключение лампы на основе показаний фоторезистора
  if (isDark) digitalWrite(LAMP_PIN, HIGH);  // Включить лампу
  else digitalWrite(LAMP_PIN, LOW);          // Выключить лампу


  lcd.print(':');            
  lcd2digits(now.second());  // Вывод минут

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

  LEDdisplay();
  SerialMonitor();
  //readSQL();
  //ServerRequest();

  delay(2000);
}