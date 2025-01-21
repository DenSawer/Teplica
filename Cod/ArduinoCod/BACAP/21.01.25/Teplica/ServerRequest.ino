void ServerRequest() {
  // Обработка данных от сервера
  int len = client.available();  // Проверка доступности данных для чтения
  if (len > 0) {
    byte buffer[80];           // Буфер для хранения полученных данных
    if (len > 80) len = 80;    // Ограничение длины данных до 80 байт
    client.read(buffer, len);  // Чтение данных в буфер
    if (printWebData) {
      Serial.write(buffer, len);  // Вывод данных в Serial Monitor
    }
    byteCount = byteCount + len;  // Увеличение счетчика принятых байт
  }
  // Отключение при завершении связи с сервером
  if (!client.connected()) {
    endMicros = micros();  // Запоминание времени окончания передачи данных
    Serial.println();
    Serial.println("Отключение.");
    client.stop();  // Остановка клиента
    Serial.print("Получено ");
    Serial.print(byteCount);  // Вывод количества полученных байт
    Serial.print(" байт за ");
    float seconds = (float)(endMicros - beginMicros) / 1000000.0;  // Расчет времени передачи данных
    Serial.print(seconds, 4);                                      // Вывод времени передачи данных
    float rate = (float)byteCount / seconds / 1000.0;              // Расчет скорости передачи данных
    Serial.print(", скорость = ");
    Serial.print(rate);  // Вывод скорости передачи данных
    Serial.print(" кбайт/сек");
    Serial.println();

    // Попытка повторного подключения к серверу
    if (client.connect(server, 80)) {
      Serial.println("Повторное подключение!");
      client.println("GET /get HTTP/1.1");  // Отправка HTTP GET-запроса
      client.println("Host: httpbin.org");  // Указание хоста
      client.println("Connection: close");  // Закрытие соединения после завершения запроса
      client.println();                     // Пустая строка для завершения заголовков запроса
      beginMicros = micros();               // Запоминание времени начала передачи данных
    } else {
      Serial.println("Ошибка повторного подключения.");
    }
  }
}