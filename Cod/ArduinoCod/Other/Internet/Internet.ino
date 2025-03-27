#include <WiFi.h>
#include <time.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>
#include <DNSServer.h>
#include <UIPEthernet.h>
#include <HTTPClient.h>

// Данные для подключения к заданной Wi-Fi сети
const char *ssid = "POCO X6 5G";
const char *password = "123456789";

// Данные для создания точки доступа
const char *apSSID = "Teplica";

// Создаем веб-сервер на порту 80
WebServer server(80);
DNSServer dnsServer;  // Для Captive Portal

// Переменные для асинхронного подключения
uint32_t wifiStartTime = 0;

#define ETHERNET_CS_PIN 5
EthernetClient client;

HardwareSerial sim800(1);

#define SIM_RX 16  // Пины подключения к sim800
#define SIM_TX 17

const char *defaultAPN = "";
const char *defaultUser = "";
const char *defaultPass = "";

const char *apnList[][4] = {
  { "25099", "internet.beeline.ru", "", "" },
  { "25001", "internet.mts.ru", "mts", "mts" },
  { "25002", "internet", "gdata", "gdata" },
  { "25020", "internet.tele2.ru", "tele2", "tele2" },
  { "25026", "internet.yota", "", "" },
  { "25096", "Inter96", "Inter96", "Inter96" }
};

struct {
  bool SIM = false;
  bool Ethernet = false;
} isPresent;

void initSIM() {
  sim800.println("AT");
  uint32_t start = millis();
  while (millis() - start < 3000) {
    if (sim800.find("OK")) {
      Serial.println("SIM YES");
      isPresent.SIM = true;
      return;
    }
  }
  Serial.println("SIM NO");
  isPresent.SIM = false;
  if (!checkSIMCard()) {
    Serial.println("SIM-карта не найдена");
    return;
  }
  if (!connectGPRS()) {
    Serial.println("Ошибка подключения к GPRS");
  } else {
    Serial.println("GPRS подключен");
    connectToInternet();
  }
}

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
}

// Функция генерации случайного MAC-адреса
void generateRandomMAC(uint8_t *mac) {
  mac[0] = 0x02;  // Локально администрируемый MAC (02:xx:xx:xx:xx:xx)
  for (int8_t i = 1; i < 6; i++) {
    mac[i] = random(0, 256);
  }
}

// Функция проверки наличия Ethernet модуля - ENC28J60
void initEthernet() {
  Ethernet.init(ETHERNET_CS_PIN);  // Инициализация ENC28J60 с указанием CS-пина
  static uint32_t start = millis();
  while (millis() - start < 3000 && Ethernet.hardwareStatus() != EthernetNoHardware)
    ;
  isPresent.Ethernet = Ethernet.hardwareStatus() != EthernetNoHardware;
}

// Функция инициализации ENC28J60 и получения IP через DHCP
bool connectEthernet() {
  if (!isPresent.Ethernet) return false;
  uint8_t mac[6];
  generateRandomMAC(mac);  // Генерируем случайный MAC-адрес

  Serial.print("Сгенерированный MAC-адрес: ");
  for (int8_t i = 0; i < 6; i++) {
    Serial.printf("%02X:", mac[i]);
  }
  Serial.println();

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Ошибка: не удалось получить IP-адрес через DHCP");
    return false;
  }

  Serial.print("IP-адрес: ");
  Serial.println(Ethernet.localIP());
  return true;
}

// Универсальная функция HTTP-запросов
String httpRequest(const char *url, const char *method = "GET", const char *payload = nullptr) {
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ошибка: нет соединения с Ethernet");
    return "";
  }

  HTTPClient http;
  http.begin(url);  // Открываем соединение с URL

  int16_t httpCode;
  if (strcmp(method, "POST") == 0 && payload) {
    http.addHeader("Content-Type", "application/json");
    httpCode = http.POST(payload);  // Отправка POST-запроса с телом
  } else {
    httpCode = http.GET();  // Отправка GET-запроса
  }

  String response = "";
  if (httpCode > 0) {
    response = http.getString();  // Получаем ответ от сервера
  } else {
    Serial.printf("Ошибка HTTP-запроса: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();  // Закрываем соединение
  return response;
}

// Функция инициализации Wi-Fi
void initWiFi() {
  WiFi.mode(WIFI_AP_STA);  // Режим работы и точки доступа и приема вифи
  WiFi.softAP(apSSID);     // Имя точки доступа
  Serial.println("Точка доступа запущена");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());

  server.onNotFound(handleRequest);  // Запуск веб-интерфейса
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());  // DNS хрень которая должна принудительно открывать наш веб-интерфейс
}

// Функция подключения к Wi-Fi с автопоиском открытых сетей
bool connectWiFi() {
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

// Попытка подключения через доступные интерфейсы
bool connectToInternet() {
  if (connectEthernet()) {
    Serial.println("** Connected via Ethernet **");
    return true;
  }
  if (connectGPRS()) {
    Serial.println("** Connected via SIM800L **");
    return true;
  }
  if (connectWiFi()) {
    Serial.println("** Connected via WiFi **");
    return true;
  }
  Serial.println("** No Internet connection **");
  return false;
}


void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, SIM_RX, SIM_TX);
  if (!SD.begin()) {
    Serial.println("Ошибка инициализации SD-карты!");
  } else {
    Serial.println("SD-карта инициализирована.");
  }
  initEthernet();
  initSIM();
  initWiFi();
  connectToInternet();
}

void loop() {
  // WiFi
  server.handleClient();
  dnsServer.processNextRequest();
  //connectWiFi();
  // СИМ
  if (isPresent.SIM) {
    if (sendHTTPRequestSIM("example.com")) {
      Serial.println("HTTP-запрос успешен");
    } else {
      Serial.println("Ошибка HTTP-запроса");
    }
    Serial.println(getMCCMNC());
  } else {
    String response = httpRequest("http://example.com");
    Serial.println("Ответ сервера: " + response);
  }
  uint32_t lastRequest = millis();
  while (millis() - lastRequest < 5000) {}
}
