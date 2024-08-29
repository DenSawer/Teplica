void SerialMonitor() {
  // --------------------Вывод информации в монитор порта-------------------------
  Serial.print("LDR Value: ");
  Serial.println(lightLevel);  // Вывод значения фоторезистора
  Serial.print("Temperature: ");
  Serial.print(microclimate.temperature);  // Вывод температуры
  Serial.println(" *C");
  Serial.print("Humidity: ");
  Serial.print(microclimate.humidity);  // Вывод влажности
  Serial.println(" %");
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoistureValue);  // Вывод влажности почвы
  Serial.println();
  Serial.print("Current Time: ");
  print2digits(now.hour());  // Вывод текущего времени (часы)
  Serial.print(':');
  print2digits(now.minute());  // Вывод текущего времени (минуты)
  Serial.print(':');
  print2digits(now.second());  // Вывод текущего времени (секунды)
  Serial.println("Core: ");
  Serial.print(xPortGetCoreID());
  Serial.println("--------------------");
}

void print2digits(uint8_t number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}