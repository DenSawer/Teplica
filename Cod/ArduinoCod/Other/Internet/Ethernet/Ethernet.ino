/*#include <UIPEthernet.h>
#include <WiFi.h>

const char* ssid = "ТВОЙ_SSID";
const char* password = "ТВОЙ_ПАРОЛЬ";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;
WiFiClient wifiClient;

bool ethConnected = false;
bool wifiConnected = false;
unsigned long dhcpStartTime;

void setup() {
  Serial.begin(115200);
  Serial.println("Запуск системы...");

  // Подключаем Wi-Fi
  Serial.println("Подключение к Wi-Fi...");
  WiFi.begin(ssid, password);
  dhcpStartTime = millis();

  // Запускаем Ethernet без блокировки
  Serial.println("Инициализация ENC28J60...");
  Ethernet.init(5);
  Ethernet.begin(mac);
}

void loop() {
  // Проверяем Wi-Fi
  if (!wifiConnected && WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.print("Wi-Fi подключен. IP: ");
    Serial.println(WiFi.localIP());
  }

  // Проверяем DHCP для Ethernet (асинхронно)
  if (!ethConnected && millis() - dhcpStartTime > 5000) {  // Ждем 5 сек
    if (Ethernet.localIP() != IPAddress(0, 0, 0, 0)) {
      ethConnected = true;
      Serial.print("Ethernet подключен. IP: ");
      Serial.println(Ethernet.localIP());
    } else {
      Serial.println("Ошибка DHCP, пробуем статический IP...");
      Ethernet.begin(mac, IPAddress(192, 168, 1, 177));
      ethConnected = true;
    }
  }

  // Выполняем HTTP-запрос
  if (wifiConnected || ethConnected) {
    sendHttpRequest();
  }

  delay(10000);
}

void sendHttpRequest() {
  Client* client;

  if (ethConnected) {
    client = &ethClient;
    Serial.println("Используем Ethernet");
  } else if (wifiConnected) {
    client = &wifiClient;
    Serial.println("Используем Wi-Fi");
  } else {
    Serial.println("Нет доступного подключения!");
    return;
  }

  if (client->connect("example.com", 80)) {
    Serial.println("Соединение установлено!");
    client->println("GET / HTTP/1.1");
    client->println("Host: example.com");
    client->println("Connection: close");
    client->println();

    delay(1000);

    Serial.println("Ответ сервера:");
    while (client->available()) {
      String line = client->readStringUntil('\n');
      Serial.println(line);
    }

    client->stop();
  } else {
    Serial.println("Ошибка подключения!");
  }
}*/
