// === ПЕРЕМЕННЫЕ ===
char* activeCO2Sensor;

// === ФУНКЦИИ ===
void goodCO2(char* str) {
  activeCO2Sensor = str;
  isPresent.CO2Sensor = true;
  char str_out[20] = "CO2 Sensor ";
  strncat(str_out, str, 9);
  showLoadingProgressBar(str_out, true);
}

void initCO2Sensor() {
  Serial.println("Инициализация CO2 датчиков...");

  if (scd30.begin()) {
    goodCO2("SCD30");
    return;
  }

  scd4x.begin(Wire, SCD41_I2C_ADDR_62);
  uint64_t serialNumber = 0;
  if (scd4x.getSerialNumber(serialNumber) == 0) {
    goodCO2("SCD4x");
    return;
  }

  showLoadingProgressBar("CO2 Sensor", false);
}

void readCO2ppm() {
  if (!isPresent.CO2Sensor) {
    Serial.println("⚠️ Нет активного CO2 датчика!");
    return;
  }

  if (activeCO2Sensor == "SCD30") {
    if (scd30.dataReady()) {
      scd30.read();
      data.CO2ppm = (uint16_t)scd30.CO2;
    }
  } else if (activeCO2Sensor == "SCD4x") {
    uint16_t co2;
    float temp, humidity;
    if (scd4x.measureAndReadSingleShot(co2, temp, humidity) == 0) {
      data.CO2ppm = co2;
    } else {
      Serial.println("Неизвестный CO2 датчик");
    }
  }
}

void readCO2TH() {
  if (!isPresent.CO2Sensor) {
    Serial.println("⚠️ Нет активного CO2 датчика!");
    return;
  }
  if (activeCO2Sensor == "SCD30") {
    if (scd30.dataReady()) {
      scd30.read();
      data.airTemp = scd30.temperature - 1.0;                                   // коррекция температуры
      data.airHum = constrain((uint8_t)(scd30.relative_humidity - 3), 0, 100);  // коррекция влажности
    }
  } else if (activeCO2Sensor == "SCD4x") {
    uint16_t co2;
    float temp, humidity;
    if (scd4x.measureAndReadSingleShot(co2, temp, humidity) == 0) {
      data.airTemp = temp - 1.2;                                 // коррекция температуры
      data.airHum = constrain((uint8_t)(humidity - 3), 0, 100);  // коррекция влажности
    }
  } else {
    Serial.println("Неизвестный CO2 датчик");
  }
}
