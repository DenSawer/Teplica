char* activeLightSensor;  // Храним только один активный датчик

void goodLight(char* str) {
  activeLightSensor = str;
  isPresent.lightSensor = true;
  char str_out[20] = "Light Sens ";
  strncat(str_out, str, 9);
  showLoadingProgressBar(str_out, true);
}


// Функция инициализации BH1750
void initLightSensors() {
  // --------------BH1750--------------- 1
  if (lightMeter.begin()) {
    if (!isnan(lightMeter.readLightLevel())) {
      goodLight("BH1750");
      return;
    }
  }
}

// Функция считывания данных с BH1750
void readLightSensors() {
  if (!isPresent.lightSensor) return;
  
  if (activeLightSensor == "BH1750") {
    data.lightLevel = lightMeter.readLightLevel();
  } else {
    Serial.print("Ошибка считывания");
  }
}