#include <SPI.h>
#include <EthernetENC.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define ENC_CS 5
uint8_t mac[6];
EthernetClient ethClient;

// Получаем MAC-адрес с WiFi
void getMAC() {
  WiFi.mode(WIFI_STA);
  delay(100);
  String macStr = WiFi.macAddress();
  for (int i = 0; i < 6; i++) {
    mac[i] = strtoul(macStr.substring(i * 3, i * 3 + 2).c_str(), nullptr, 16);
  }
}

// Проверка наличия ENC28J60 (ответ по SPI)
bool isENC28J60Present() {
  // Пытаемся прочитать регистр, как минимум должен быть ненулевой ответ
  uint8_t testMac[6];
  getMACFromWiFi(testMac);
  Ethernet.init(ENC_CS);
  Ethernet.begin(testMac);
  delay(500);
  return Ethernet.hardwareStatus() == EthernetNoHardware ? false : true;
}

// Проверка наличия линка
bool isEthernetLinkUp() {
  return Ethernet.linkStatus() == LinkON;
}

// Подключение по DHCP
bool connectEthernetDHCP(uint8_t* mac) {
  Ethernet.init(ENC_CS);
  return Ethernet.begin(mac);
}

// Универсальная функция HTTP-запросов
String httpRequest(const String& url, const String& method = "GET", const String& payload = "") {
  HTTPClient http;
  http.begin(url);
  http.setTimeout(5000);  // 5 секунд на таймаут

  int httpCode = -1;
  String response = "";

  if (method == "POST") {
    http.addHeader("Content-Type", "application/json");
    httpCode = http.POST(payload);
  } else {
    httpCode = http.GET();
  }

  if (httpCode > 0) {
    response = http.getString();
    Serial.printf("HTTP %s-код: %d\n", method.c_str(), httpCode);
  } else {
    Serial.printf("Ошибка HTTP: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return response;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  getMACFromWiFi(mac);

  if (!isENC28J60Present()) {
    Serial.println("ENC28J60 не обнаружен!");
    return;
  }

  if (!connectEthernetDHCP(mac)) {
    Serial.println("Ошибка подключения по DHCP!");
    return;
  }

  if (!isEthernetLinkUp()) {
    Serial.println("Нет линка Ethernet!");
    return;
  }

  Serial.print("Подключено, IP: ");
  Serial.println(Ethernet.localIP());

  String response = httpRequest("http://example.com");
  Serial.println("Ответ сервера:");
  Serial.println(response);
}

void loop() {
  // основной цикл пуст
}
