#include <WiFi.h>
uint32_t espID;

void setup() {
  Serial.begin(115200);

  espID = (uint32_t)ESP.getEfuseMac();  // младшие 4 байта уникального чип-ID
  Serial.print("espID: ");
  Serial.println(espID, HEX);

  /*#include <CRC32.h>

const char base62chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void generateEspID() {
  // CRC32 от MAC
  CRC32 crc;
  crc.update(mac, 6);
  uint32_t hash = crc.final();

  // Преобразуем в Base62 и записываем в espID (ровно 6 символов)
  for (int i = 5; i >= 0; i--) {
    espID[i] = base62chars[hash % 62];
    hash /= 62;
  }
  espID[6] = '\0'; // Завершающий символ строки
}*/

  WiFi.mode(WIFI_STA);  // Включаем Wi-Fi в режиме клиента (обязательно)
  delay(100);           // Можно добавить маленькую паузу
  Serial.print("\nNEW ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {}
