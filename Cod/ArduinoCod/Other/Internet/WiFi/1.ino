/*#include <WiFi.h>
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

// Функции для управления Wi-Fi
void connectWiFi();
void startAccessPoint();
void handleRequest();
void serveFile(String path);

void setup() {
  Serial.begin(115200);  // Запускаем Serial для отладки
  if (!SD.begin()) {
    Serial.println("Ошибка инициализации SD-карты!");
  } else {
    Serial.println("SD-карта инициализирована.");
  }

  WiFi.mode(WIFI_AP_STA);  // Одновременный режим точки доступа и клиента
  connectWiFi();           // Подключение к Wi-Fi
  startAccessPoint();      // Запуск точки доступа

  server.onNotFound(handleRequest);
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());  // Captive Portal перенаправляет все запросы на наш сервер
}

void loop() {
  server.handleClient();           // Обрабатываем запросы к веб-серверу
  dnsServer.processNextRequest();  // Обрабатываем DNS-запросы для Captive Portal
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

// Функция запуска точки доступа
void startAccessPoint() {
  WiFi.softAP(apSSID);
  Serial.println("Точка доступа запущена");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());
}

// Обработчик запросов
void handleRequest() {
  String path = server.uri();
  if (path == "/") path = "/index.html";
  serveFile(path);
}

// Функция отправки файлов с SD-карты
void serveFile(String path) {
  File file = SD.open(path);
  if (!file) {
    server.send(404, "text/plain", "Файл не найден");
    return;
  }

  String contentType = "text/html";
  if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".png")) contentType = "image/png";
  else if (path.endsWith(".jpg")) contentType = "image/jpeg";
  else if (path.endsWith(".ico")) contentType = "image/x-icon";

  server.streamFile(file, contentType);
  file.close();
}

*/