// Функция инициализации DS18B20
void initSoilTemp() {
  sensors.begin();
  sensorCount = sensors.getDeviceCount();
  if (sensorCount > 0) {
    isPresent.soilTempSensor = true;
    char str_out[20];
    sprintf(str_out, "Soil Temp Sensor %d", sensorCount);
    showLoadingProgressBar(str_out, true);
  } else {
    isPresent.soilTempSensor = false;
    showLoadingProgressBar("Soil Temp Sensor", false);
  }
}

// Функция вывода средней температуры с датчиков
void readSoilTemp() {
  if (isPresent.soilTempSensor) {
    sensors.requestTemperatures();
    float sumTemp = 0;
    for (int i = 0; i < sensorCount; i++) {
      float temp = sensors.getTempCByIndex(i);
      sumTemp += temp;
    }
    data.soilTemp = sumTemp / sensorCount;
  }
}

// ================ ВЛАЖНОСТЬ ==================

// Калибровочные значения (подбираются вручную)
#define dryADC 3200  // Значение в сухой почве
#define wetADC 1500  // Значение в очень влажной почве

void initSoilMois() {
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  // Проверка наличия датчика
  int16_t testValue = analogRead(SOIL_MOISTURE_PIN);
  if (testValue == 0 || testValue >= 4095) {
    showLoadingProgressBar("Soil Mois Sensor", false);
    isPresent.soilMoisSensor = false;
  } else {
    showLoadingProgressBar("Soil Mois Sensor", true);
    isPresent.soilMoisSensor = true;
  }
}
// Функция вывода данных
void readSoilMois() {
  if (!isPresent.soilMoisSensor) return;
  int16_t adcValue = analogRead(SOIL_MOISTURE_PIN);
  data.soilMois = static_cast<uint8_t>(constrain(100 * (dryADC - adcValue) / (dryADC - wetADC), 0, 100));
/*
  Serial.print("ADC: ");
  Serial.print(adcValue);
  Serial.print(" | Влажность: ");
  Serial.print(data.soilMois);
  Serial.println("%");

  if (data.soilMois <= 10) {
    Serial.println("💀 Почва полностью суха! Требуется полив!");
  } else if (data.soilMois <= 40) {
    Serial.println("🌱 Почва суховата. Можно полить.");
  } else if (data.soilMois <= 70) {
    Serial.println("🌿 Влажность в норме.");
  } else {
    Serial.println("💦 Почва слишком влажная! Возможен застой воды.");
  }*/
}
