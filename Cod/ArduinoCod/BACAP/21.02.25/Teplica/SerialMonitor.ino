void SerialMonitor() {
  // --------------------Вывод информации в монитор порта-------------------------
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.print("Current time: ");
    Serial.println(&timeinfo, "%Y/%m/%d %H:%M:%S");  // Время
  } else {
    Serial.println("Failed to obtain time");  // Ошибка времени
  }
  Serial.print("LDR Value: ");
  Serial.println(lightLevel);  // Вывод значения фоторезистора
  Serial.print("Temperature: ");
  Serial.print(microclimate.temperature);  // Вывод температуры
  Serial.println(" *C");
  Serial.print("Humidity: ");
  Serial.print(microclimate.humidity);  // Вывод влажности
  Serial.println(" %");
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);  // Вывод влажности почвы
  //printCPULoad();
  Serial.println("--------------------");
}

void print2digits(uint8_t number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

/*
void printCPULoad() {
  static uint32_t prevIdle0 = 0, prevIdle1 = 0;
  static uint32_t prevTime = 0;

  TaskStatus_t taskStatus[2];
  UBaseType_t taskCount;
  uint32_t totalRunTime;

  taskCount = uxTaskGetSystemState(taskStatus, 2, &totalRunTime);

  uint32_t idle0 = taskStatus[0].ulRunTimeCounter;  // Время простоя ядра 0
  uint32_t idle1 = taskStatus[1].ulRunTimeCounter;  // Время простоя ядра 1
  uint32_t currentTime = millis();

  if (prevTime > 0 && totalRunTime > 0) {
    float elapsedTime = (currentTime - prevTime) * 1000.0;  // в микросекундах

    float load0 = 100.0 - ((idle0 - prevIdle0) * 100.0 / elapsedTime);
    float load1 = 100.0 - ((idle1 - prevIdle1) * 100.0 / elapsedTime);

    Serial.printf("CPU0 Load: %.2f%% | CPU1 Load: %.2f%%\n", load0, load1);
  }

  prevIdle0 = idle0;
  prevIdle1 = idle1;
  prevTime = currentTime;
}*/
