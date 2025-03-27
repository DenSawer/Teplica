// Функция сохранения настроек в JSON
void saveSettings() {
  if (!isPresent.SD) return;

  File file = SD.open("/settings.json", FILE_WRITE);
  if (!file) {
    Serial.println("Ошибка записи в settings.json");
    return;
  }

  JsonDocument doc;
  doc["ssid_WiFi"] = settings.ssid_WiFi;
  doc["password_WiFi"] = settings.password_WiFi;
  doc["gUTC"] = settings.gUTC;
  doc["isCorF"] = settings.isCorF;

  serializeJson(doc, file);
  file.close();
  Serial.println("Настройки сохранены.");
}

// Функция загрузки настроек из JSON
void loadSettings() {
  if (!isPresent.SD) return;

  File file = SD.open("/settings.json", FILE_READ);
  if (!file) {
    Serial.println("Файл настроек не найден, создаем...");
    saveSettings();  // Создаем файл с текущими значениями
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("Ошибка загрузки настроек!");
    return;
  }

  settings.ssid_WiFi = doc["ssid_WiFi"].as<String>();
  settings.password_WiFi = doc["password_WiFi"].as<String>();
  settings.gUTC = doc["gUTC"].as<uint8_t>();
  settings.isCorF = doc["isCorF"].as<bool>();
  Serial.println("Настройки загружены.");
}

// Функция сохранения данных в JSON-формате (MongoDB-совместимый формат)
void saveData() {
  if (!isPresent.SD) return;

  File file = SD.open("/data.json", FILE_APPEND);
  if (!file) {
    Serial.println("Ошибка записи в data.json");
    return;
  }

  JsonDocument doc;
  doc["airTemp"] = data.airTemp;
  doc["soilTemp"] = data.soilTemp;
  doc["aitHum"] = data.aitHum;
  doc["soilMois"] = data.soilMois;
  doc["lightLevel"] = data.lightLevel;
  doc["isCool"] = data.isCool;
  doc["isHeat"] = data.isHeat;
  doc["isDark"] = data.isDark;

  serializeJson(doc, file);
  file.println();
  file.close();

  Serial.println("Данные сохранены.");
}
