#include <SPI.h>
#include <EthernetENC.h>
#include <WiFi.h>

#define ENC_CS 5
uint8_t mac[6];
bool isEth = false;

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
void initEthernet() {
  // Пытаемся прочитать регистр, как минимум должен быть ненулевой ответ
  Ethernet.init(ENC_CS);
  Ethernet.begin(mac);
  delay(500);
  isEth = Ethernet.hardwareStatus() == EthernetNoHardware ? false : true;
}

// Подключение по DHCP
bool connectEthernet() {
  Ethernet.init(ENC_CS);
  return Ethernet.begin(mac);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  getMAC();

  if (!isENC28J60Present()) {
    Serial.println("ENC28J60 не обнаружен!");
    return;
  }

  if (!connectEthernetDHCP()) {
    Serial.println("Ошибка подключения по DHCP!");
    return;
  }

  Serial.print("Подключено, IP: ");
  Serial.println(Ethernet.localIP());

}

void loop() {
  // основной цикл пуст
}