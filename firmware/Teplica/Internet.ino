String getMCCMNC() {
  sim800.println("AT+COPS?");
  uint32_t start = millis();
  while (millis() - start < 3000) {
    if (sim800.find("+COPS: ")) {
      sim800.readStringUntil(',');
      sim800.readStringUntil(',');
      String mccmnc = sim800.readStringUntil('"');
      mccmnc = sim800.readStringUntil('"');
      return mccmnc;
    }
  }
  return "";
}

bool connectGPRS() {
  if (!isPresent.SIM) return false;
  const char *apn = defaultAPN;
  const char *user = defaultUser;
  const char *pass = defaultPass;

  // Определение MCCMNC
  String mccmnc = getMCCMNC();

  for (uint8_t i = 0; i < sizeof(apnList) / sizeof(apnList[0]); i++) {
    if (mccmnc == apnList[i][0]) {
      apn = apnList[i][1];
      user = apnList[i][2];
      pass = apnList[i][3];
      break;
    }
  }

  sim800.print("AT+CGDCONT=1,\"IP\",\"");
  sim800.print(apn);
  sim800.println("\"");
  uint32_t start = millis();
  while (millis() - start < 1000) {
    if (sim800.find("OK")) {
      Serial.println("APN установлен");
      break;
    }
  }

  Serial.println("[INFO] Настройка GPRS-соединения...");
  sim800.print("AT+CSTT=\"");
  sim800.print(apn);
  sim800.print("\",\"");
  sim800.print(user);
  sim800.print("\",\"");
  sim800.print(pass);
  sim800.println("\"");

  start = millis();
  while (millis() - start < 2000) {
    if (sim800.find("OK")) {
      Serial.println("[OK] GPRS настроен");
      break;
    }
  }

  Serial.println("[INFO] Подключение к сети...");
  sim800.println("AT+CGATT=1");
  start = millis();
  while (millis() - start < 2500) {
    if (sim800.find("OK")) {
      Serial.println("[OK] Подключение к сети выполнено");
      break;
    }
  }

  Serial.println("[INFO] Активация соединения...");
  sim800.println("AT+CIICR");
  start = millis();
  while (millis() - start < 5000) {
    if (sim800.find("OK")) {
      Serial.println("[OK] Соединение активировано");
      break;
    }
  }

  Serial.println("[INFO] Получение IP-адреса...");
  sim800.println("AT+CIFSR");
  bool connected = sim800.find(".");

  if (connected) {
    Serial.println("[SUCCESS] Подключение к интернету успешно");
  } else {
    Serial.println("[ERROR] Не удалось получить IP-адрес");
  }

  return connected;
}

// Проверка наличия ENC28J60 (ответ по SPI)
void initEthernet() {
  // Пытаемся прочитать регистр, как минимум должен быть ненулевой ответ
  //Ethernet.init(ETHERNET_CS_PIN);
  //Ethernet.begin(mac);
  delay(500);
  isPresent.Ethernet = true;
  showLoadingProgressBar("Ethernet", isPresent.Ethernet);
  /*isPresent.Ethernet = Ethernet.hardwareStatus() == EthernetNoHardware ? false : true;
  showLoadingProgressBar("Ethernet", isPresent.Ethernet);*/
}

// Подключение по DHCP
bool connectEthernet() {
  if (!isPresent.Ethernet) return false;
  Ethernet.init(ETHERNET_CS_PIN);
  return Ethernet.begin(mac);
}

/*
bool sendHTTPRequestSIM(const char *host) {
  sim800.print("AT+CIPSTART=\"TCP\",\"");
  sim800.print(host);
  sim800.println("\",\"80\"");

  uint32_t start = millis();
  while (millis() - start < 5000) {
    if (sim800.find("CONNECT OK")) break;
  }

  sim800.println("AT+CIPSEND");
  start = millis();
  while (millis() - start < 2000) {
    if (sim800.find(">")) break;
  }

  sim800.print("GET / HTTP/1.1\r\nHost: ");
  sim800.print(host);
  sim800.print("\r\nConnection: close\r\n\r\n");
  sim800.write(26);

  start = millis();
  while (millis() - start < 5000) {
    if (sim800.find("200 OK")) return true;
  }
  return false;
}*/

// Функция инициализации Wi-Fi
void initWiFi() {
  WiFi.mode(WIFI_AP_STA);         // Режим работы и точки доступа и приема вифи
  WiFi.softAP(settings.ssid_AP);  // Имя точки доступа
  Serial.println("Точка доступа запущена");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());

  server.onNotFound(handleRequest);  // Запуск веб-интерфейса
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());  // DNS хрень которая должна принудительно открывать наш веб-интерфейс
  showLoadingProgressBar("Wi-Fi", true);
}

// Функция подключения к Wi-Fi с автопоиском открытых сетей
bool connectWiFi() {
  WiFi.begin(settings.ssid_WiFi, settings.password_WiFi);
  Serial.print("Подключение к WiFi");

  uint32_t startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nУспешное подключение к Wi-Fi!");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("\nОшибка подключения. Поиск открытых сетей...");
  WiFi.disconnect();
  int8_t bestRSSI = -100;
  String bestSSID = "";

  int8_t networks = WiFi.scanNetworks();
  for (int8_t i = 0; i < networks; i++) {
    if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN && WiFi.RSSI(i) > bestRSSI) {
      bestRSSI = WiFi.RSSI(i);
      bestSSID = WiFi.SSID(i);
    }
  }

  if (bestSSID != "") {
    Serial.println("Подключение к лучшей открытой сети: " + bestSSID);
    WiFi.begin(bestSSID.c_str());
    startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    return true;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nОткрытых сетей не найдено.");
  }
  return false;
}

// Обработчик запросов для веб-интерфейса
// Обработчик всех HTTP-запросов и выдача файлов с SD-карты
void handleRequest() {
  String path = server.uri();  // Получаем запрошенный путь, например "/index.html" или "/что-угодно"

  File file = SD.open(path);  // Пытаемся открыть файл с этим именем на SD

  if (!file) {
    // Если файл не найден — это может быть случай DNS-редиректа (как в Captive Portal)
    // Отдаём принудительно index.html для отображения веб-интерфейса
    file = SD.open("/index.html");
    if (!file) {
      // Если даже index.html не найден — отправляем ошибку
      server.send(500, "text/plain", "Index file missing");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
    return;
  }

  // Определяем тип содержимого (MIME) по расширению файла
  String contentType = "text/plain";
  if (path.endsWith(".html")) contentType = "text/html";
  else if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".png")) contentType = "image/png";
  else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) contentType = "image/jpeg";
  else if (path.endsWith(".ico")) contentType = "image/x-icon";

  // Отправляем файл клиенту
  server.streamFile(file, contentType);
  file.close();
}

// Попытка подключения через доступные интерфейсы
void connectToInternet() {
  if (connectWiFi()) {
    Serial.println("** Connected via WiFi **");
    InternetFromSIM = false;
    isConnect = true;
    return;
  }
  /*if (connectEthernet()) {
    Serial.println("** Connected via Ethernet **");
    InternetFromSIM = false;
    isConnect = true;
    return;
  }
  if (connectGPRS()) {
    Serial.println("** Connected via SIM800L **");
    InternetFromSIM = true;
    isConnect = true;
    return;
  }*/

  Serial.println("** No Internet connection **");
  InternetFromSIM = false;
  isConnect = false;
  return;
}
