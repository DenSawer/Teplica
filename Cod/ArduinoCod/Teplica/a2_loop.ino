void loop() {
  unsigned long currentMillis = millis();                                  // Текущее время
  static unsigned long lastUpdate1 = 0, lastUpdate2 = 0, lastUpdate3 = 0;  // Время последнего обновления

  if (knopka) {
    if (currentMillis - lastUpdate1 > 30 * 60000) {
      lastUpdate1 = currentMillis;
      knopka = false;
      stopAP();
    }
  } else {
    knopkaState = digitalRead(KNOPKA_PIN);  // читаем кнопку
    if (knopkaState) {
      knopka = true;
      startAP();
    }
  }

  if (currentMillis - lastUpdate2 > 1000 && knopka) {
    lastUpdate2 = currentMillis;
    // WiFi для веб-интерфейса
    server.handleClient();
    dnsServer.processNextRequest();
    displayMonitor();
    serialMonitor();
  }

  if (currentMillis - lastUpdate3 > 5000) {
    lastUpdate3 = currentMillis;
    readCO2ppm();
    readAirSensors(); 
    readSoilMois();
    readSoilTemp();
    readLightSensors();
    updateLocalTime();  // Обновление времени
    saveData();         // Сохранение данных
  }
}

void startAP() {
  WiFi.mode(WIFI_AP_STA);  // если вдруг был WIFI_STA
  WiFi.softAP(settings.ssid_AP);
}

void stopAP() {
  WiFi.softAPdisconnect(true);  // отключает только AP
}
