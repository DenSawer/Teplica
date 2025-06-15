/*#include <SPI.h>
#include <EthernetENC.h>
#include <WiFi.h>
#include <EthernetClient.h>

#define ENC_CS 5  // Chip Select на пине D5 (GPIO5)

uint8_t mac[6];  // MAC-адрес
EthernetClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Установка CS пина до инициализации Ethernet
  Ethernet.init(ENC_CS);

  // Получаем MAC от WiFi
  WiFi.mode(WIFI_STA);
  delay(100);
  String macStr = WiFi.macAddress();

  for (int i = 0; i < 6; i++) {
    mac[i] = strtoul(macStr.substring(i * 3, i * 3 + 2).c_str(), nullptr, 16);
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Инициализация Ethernet (DHCP)
  if (Ethernet.begin(mac)) {
    Serial.println("Ethernet подключён с DHCP!");
    Serial.print("IP адрес: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("DHCP не работает, пробуем статический IP...");
    IPAddress ip(192, 168, 1, 177);
    IPAddress dns(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    Ethernet.begin(mac, ip, dns, gateway, subnet);
    Serial.print("Статический IP: ");
    Serial.println(Ethernet.localIP());
  }

  delay(1000);

  // Простой HTTP-запрос
  if (client.connect("example.com", 80)) {
    Serial.println("Подключено к example.com");

    client.println("GET / HTTP/1.1");
    client.println("Host: example.com");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Ошибка подключения к серверу");
  }
}

void loop() {
  // Печатаем ответ сервера
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // Закрываем соединение, если всё получено
  if (!client.connected()) {
    Serial.println();
    Serial.println("Соединение закрыто.");
    client.stop();
    delay(10000);  // ждём 10 секунд до следующего запроса
  }
}
*/