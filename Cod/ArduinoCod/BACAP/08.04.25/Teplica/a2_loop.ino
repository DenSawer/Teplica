void loop() {
  // WiFi для веб-интерфейса
  server.handleClient();
  dnsServer.processNextRequest();

  readCO2ppm();
  readAirSensors();       // Инициализация DHT22
  readSoilMois();
  readSoilTemp();
  readLightSensors();

  updateLocalTime();   // Обновление времени

  displayMonitor();
  serialMonitor();
  saveData();

  delay(1000);
}
