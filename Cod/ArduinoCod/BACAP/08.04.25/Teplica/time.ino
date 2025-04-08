#define TIMEOUT 500  // Таймаут ожидания в миллисекундах

// Настройка времени через NTP
bool setupTimeNTP() {
  if (connectWiFi()) {
    configTime(3600 * settings.gUTC, 0, NTP_SERVER);  // часовой пояс, летнее время, нтп сервер
    const time_t currentTime = time(nullptr);
    // Если время корректное (порог можно заменить на нужное значение)
    if (currentTime > 1670000000UL) {
      lastSyncTime = currentTime;
      return true;
    }
  }
  return false;
}

// Получение времени с SIM800L
bool getTimeFromSIM() {
  sim800.println("AT+CCLK?");
  delay(100);
  const String response = sim800.readString();

  // Используем uint16_t для года и uint8_t для остальных компонентов
  uint16_t year = 0;
  uint8_t month = 0, day = 0, hour = 0, minute = 0, second = 0;

  // Используем соответствующие спецификаторы формата:
  // %hu для unsigned short (uint16_t) и %hhu для unsigned char (uint8_t)
  if (sscanf(response.c_str(), "\r\n+CCLK: \"%hu/%hhu/%hhu,%hhu:%hhu:%hhu\"", &year, &month, &day, &hour, &minute, &second) == 6) {
    struct tm timeinfo = { 0 };
    // Предполагается, что SIM возвращает год в формате, например, 21 для 2021 года.
    // В структуре tm год хранится как количество лет с 1900, поэтому прибавляем 100.
    timeinfo.tm_year = year + 100;
    timeinfo.tm_mon = month - 1;  // месяцы от 0 до 11
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;

    const time_t newTime = mktime(&timeinfo);
    if (newTime > 1670000000UL) {  // Проверка корректности времени
      struct timeval tv = { newTime, 0 };
      settimeofday(&tv, NULL);
      lastSyncTime = newTime;
      return true;
    }
  }
  return false;
}

// Обновление локального времени (проверка, пора ли обновлять)
void updateLocalTime() {
  if (time(nullptr) < 1670000000UL) {
    static uint32_t lastAttemptTime = 0;  // Создается только в этом блоке и исчезает после установки времени
    if (millis() - lastAttemptTime >= 1000) {
      lastAttemptTime = millis();
      if (getTimeFromSIM() || setupTimeNTP()) return;  // Если удалось синхронизировать, выходим
    }
  }

  const time_t currentTime = time(nullptr);
  if (currentTime - lastSyncTime < timeSyncInterval) return;  // Проверка прошло ли уже время обновиться
  if (getTimeFromSIM()) return;                               // Обновление через SIM
  if (setupTimeNTP()) return;                                 // Обновление через NTP
  lastSyncTime = currentTime;                                 // Если обе попытки неудачны, обновляем lastSyncTime
}
