// Функция инициализации SD-карты
void initSD() {
  uint32_t start = millis();
  while (millis() - start < 3000 && !isPresent.SD) {
    if (SD.begin(SD_CS_PIN)) {
      isPresent.SD = true;
      showLoadingProgressBar("SD", true);
      return;
    }
  }
  isPresent.SD = false;
  showLoadingProgressBar("SD", false);
}

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
  Serial.println("Настройки загружены.");
}

// Глобальные переменные
int lastSavedHour = -1;
int lastSavedHalfHour = -1;

void saveData() {
  if (!isPresent.SD) return;

  // Получаем текущее время
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  int currentHour = timeinfo.tm_hour;
  int currentHalfHour = (timeinfo.tm_min < 30) ? 0 : 1;

  // Проверяем: прошло ли новое 30-минутное окно
  if (currentHour == lastSavedHour && currentHalfHour == lastSavedHalfHour) {
    return;  // уже сохраняли в этом интервале
  }

  // Обновляем контрольные значения
  lastSavedHour = currentHour;
  lastSavedHalfHour = currentHalfHour;

  // Форматируем дату и время
  char dateStr[11];  // DD/MM/YYYY
  char timeStr[9];   // HH:MM:SS
  strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  // Открываем файл
  File file = SD.open("/data.json", FILE_APPEND);
  if (!file) {
    Serial.println("Ошибка записи в data.json");
    file = SD.open("/data.json", FILE_WRITE);
    if (!file) {
      Serial.println("Не удалось создать файл");
      return;
    }
  }

  // Формируем JSON
  JsonDocument doc;
  doc["date"] = dateStr;
  doc["time"] = timeStr;
  doc["airTemp"] = data.airTemp;
  doc["soilTemp"] = data.soilTemp;
  doc["airHum"] = data.airHum;
  doc["soilMois"] = data.soilMois;
  doc["lightLevel"] = data.lightLevel;
  doc["CO2ppm"] = data.CO2ppm;

  serializeJson(doc, file);
  file.println();
  file.close();

  Serial.println("Данные сохранены раз в 30 минут.");
}