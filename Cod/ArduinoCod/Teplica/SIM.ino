// Функция инициализации SIM-карты
void initSIM() {
  sim800.println("AT");
  uint32_t start = millis();
  while (millis() - start < 3000) {
    if (sim800.find("OK")) {
      isPresent.SIM = true;
      showLoadingProgressBar("SIM", true);
      getTimeFromSIM();
      if (!checkSIMCard()) {
        Serial.println("SIM-карта не найдена");
        return;
      }
      return;
    }
  }
  isPresent.SIM = false;
  showLoadingProgressBar("SIM", false);
}

// Функция проверки наличия SIM-карты
bool checkSIMCard() {
  if (!isPresent.SIM) return false;
  sim800.println("AT+CPIN?");
  uint32_t start = millis();
  while (millis() - start < 3000) {
    if (sim800.find("READY")) {
      return true;
    }
  }
  return false;
}