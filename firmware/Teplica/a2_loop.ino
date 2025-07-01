void loop() {
  unsigned long currentMillis = millis();                                  // Текущее время
  static unsigned long lastUpdate1 = 0, lastUpdate2 = 0, lastUpdate3 = 0;  // Время последнего обновления
  static bool start = true;
  // WiFi для веб-интерфейса
  server.handleClient();
  dnsServer.processNextRequest();

  if (knopka) {
    if (currentMillis - lastUpdate1 > 30 * 60000 || digitalRead(KNOPKA_PIN) == HIGH) {
      lcd.noBacklight();
      lcd.clear();
      stopAP();
      while (digitalRead(KNOPKA_PIN) == HIGH)
        ;
      delay(1000);
      knopka = false;
    }
  } else {
    // читаем кнопку
    if (digitalRead(KNOPKA_PIN) == HIGH) {
      lastUpdate1 = currentMillis;
      start = true;
      lcd.backlight();
      displayMonitor();
      startAP();
      while (digitalRead(KNOPKA_PIN) == HIGH)
        ;
      delay(1000);
      knopka = true;
    }
  }

  if (currentMillis - lastUpdate2 > 5000 && knopka || start) {
    lastUpdate2 = currentMillis;
    read();
    displayMonitor();
    serialMonitor();
  }

  if (currentMillis - lastUpdate3 > 1 * 60000 || start) {
    lastUpdate3 = currentMillis;
    updateLocalTime();  // Обновление времени
    saveData();         // Сохранение данных
  }
  start = false;
}

void startAP() {
  WiFi.mode(WIFI_AP_STA);  // если вдруг был WIFI_STA
  WiFi.softAP(settings.ssid_AP);
  server.onNotFound(handleRequest);  // Запуск веб-интерфейса
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());  // DNS хрень которая должна принудительно открывать наш веб-интерфейс
}

void stopAP() {
  WiFi.softAPdisconnect(true);  // отключает только AP
}

void read() {
  readCO2ppm();
  readAirSensors();
  readSoilMois();
  readSoilTemp();
  readLightSensors();
}
