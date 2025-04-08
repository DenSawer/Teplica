char* activeAirSensor;  // Храним только один активный датчик

void goodAir(char* str) {
  activeAirSensor = str;
  isPresent.airSensor = true;
  char str_out[20] = "Air Sensor ";
  strncat(str_out, str, 9);
  showLoadingProgressBar(str_out, true);
}

// === ИНИЦИАЛИЗАЦИЯ ДАТЧИКОВ ===
void initAirSensor() {
  Serial.println("Инициализация датчиков...");

  // --------------SHT4x--------------- 1
  if (sht4.begin()) {
    sensors_event_t humidity, temp_event;
    sht4.getEvent(&humidity, &temp_event);
    if (!isnan(temp_event.temperature) && !isnan(humidity.relative_humidity)) {
      goodAir("SHT4x");
      return;
    }
  }
  // --------------SHT3x--------------- 2
  if (sht3.begin(0x44) || sht3.begin(0x45)) {
    if (!isnan(sht3.readTemperature()) && !isnan(sht3.readHumidity())) {
      goodAir("SHT3x");
      return;
    }
  }
  // --------------SHT2x--------------- 3
  if (sht2.begin()) {
    if (!isnan(sht2.getTemperature()) && !isnan(sht2.getHumidity())) {
      goodAir("SHT2x");
      return;
    }
  }
  // --------------DHT22--------------- 4
  dht.begin();
  if (!isnan(dht.readTemperature()) && !isnan(dht.readHumidity())) {
    goodAir("DHT22");
    return;
  }
  // --------------BME280-------------- 5
  if (bme.begin(0x76)) {
    if (!isnan(bme.readTemperature()) && !isnan(bme.readHumidity())) {
      goodAir("BME280");
      return;
    }
  }
  // --------------CO2Sensors-------------- 6
  if (activeCO2Sensor == "SCD30" || activeCO2Sensor == "SCD4x") {
    goodAir(activeCO2Sensor);
  }
  // --------------Конец---------------
  showLoadingProgressBar("Air Sensor", false);
}

// === ЧТЕНИЕ ДАННЫХ С ВЫБРАННОГО ДАТЧИКА ===
void readAirSensors() {
  if (!isPresent.airSensor) {
    Serial.println("⚠️ Нет активного датчика!");
    return;
  }
  if (activeAirSensor == "SCD30" || activeAirSensor == "SCD4x") {
    readCO2TH();
    // --------------SHT4x---------------
  } else if (activeAirSensor == "SHT4x") {
    sensors_event_t humidity, temp_event;
    sht4.getEvent(&humidity, &temp_event);
    data.airTemp = temp_event.temperature - 0.3;
    data.airHum = constrain((uint8_t)(humidity.relative_humidity - 1), 0, 100);  // смещение и выравнивание
    // --------------SHT3x---------------
  } else if (activeAirSensor == "SHT3x") {
    data.airTemp = sht3.readTemperature() - 0.4;
    data.airHum = constrain((uint8_t)(sht3.readHumidity() - 3), 0, 100);
    // --------------SHT2x---------------
  } else if (activeAirSensor == "SHT2x") {
    data.airTemp = sht2.getTemperature() - 0.4;
    data.airHum = constrain((uint8_t)(sht2.getHumidity() - 2), 0, 100);
    // --------------BME280--------------
  } else if (activeAirSensor == "BME280") {
    data.airTemp = bme.readTemperature() - 0.7;
    data.airHum = constrain((uint8_t)(bme.readHumidity() - 6), 0, 100);
    // --------------DHT22---------------
  } else if (activeAirSensor == "DHT22") {
    data.airTemp = dht.readTemperature() - 0.5;
    data.airHum = constrain((uint8_t)(dht.readHumidity() - 5), 0, 100);
    // ---------------NONE---------------
  } else {
    Serial.println("Ошибка подключения");
    return;
  }
}
