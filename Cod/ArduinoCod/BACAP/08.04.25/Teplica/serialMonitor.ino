void serialMonitor() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("===================================");
    Serial.print("📅 Время: ");
    Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");

    Serial.println("📊 Показания датчиков:");
    Serial.printf("🌡️  Температура воздуха:     %.2f °C\n", data.airTemp);
    Serial.printf("🌡️  Температура почвы:       %.2f °C\n", data.soilTemp);
    Serial.printf("💧 Влажность воздуха:         %u %%\n", data.airHum);
    Serial.printf("🌱 Влажность почвы:           %d %%\n", data.soilMois);
    Serial.printf("💡 Освещенность:              %u Лк\n", data.lightLevel);
    Serial.printf("🟢 Уровень CO₂:               %u ppm\n", data.CO2ppm);
    Serial.println("===================================");
  } else {
    Serial.println("❌ Ошибка получения времени");
  }
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
