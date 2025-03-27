#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>
#include <DNSServer.h>
#include <stdint.h>

// Данные для подключения к заданной Wi-Fi сети
const char* ssid = "POCO X6 5G";
const char* password = "123456789";

// Данные для создания точки доступа
const char* apSSID = "Teplice";

// Создаем веб-сервер на порту 80
WebServer server(80);
DNSServer dnsServer;  // Для Captive Portal

// Переменные для асинхронного подключения
uint32_t wifiStartTime = 0;

void setup() {
  Serial.begin(115200);
  if (!SD.begin()) {
    Serial.println("Ошибка инициализации SD-карты!");
  } else {
    Serial.println("SD-карта инициализирована.");
  }

  initWiFi();
  connectWiFi();
}

void loop() {
  server.handleClient();
  dnsServer.processNextRequest();
  //connectWiFi();
  static uint32_t startTime = millis();
  if (millis() - startTime > 500) {
    Serial.print("1");
    startTime = millis();
  }
}

// Функция инициализации Wi-Fi
void initWiFi() {
  WiFi.mode(WIFI_AP_STA); // Режим работы и точки доступа и приема вифи
  WiFi.softAP(apSSID); // Имя точки доступа
  Serial.println("Точка доступа запущена");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());

  server.onNotFound(handleRequest); // Запуск веб-интерфейса
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP()); // DNS хрень которая должна принудительно открывать наш веб-интерфейс
}

// Функция подключения к Wi-Fi с автопоиском открытых сетей
void connectWiFi() {
  WiFi.begin(ssid, password);
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
    return;
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
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nОткрытых сетей не найдено.");
  }
}

// Обработчик запросов
void handleRequest() {
  String path = server.uri();
  if (path == "/") path = "/index.html";
  serveFile(path);
}

// Функция открытия файлов сайта с SD-карты
void serveFile(String path) {
  File file = SD.open(path);
  if (!file) {
    server.send(404, "text/plain", "File error");
    return;
  }

  String contentType = "text/html";
  if (path.endsWith(".html")) contentType = "text/html";
  else if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".png")) contentType = "image/png";
  else if (path.endsWith(".jpg")) contentType = "image/jpeg";
  else if (path.endsWith(".ico")) contentType = "image/x-icon";
  else if (path.endsWith(".ttf")) contentType = "font/ttf";
  else if (path.endsWith(".woff")) contentType = "font/woff";
  else if (path.endsWith(".woff2")) contentType = "font/woff2";

  server.streamFile(file, contentType);
  file.close();
}
