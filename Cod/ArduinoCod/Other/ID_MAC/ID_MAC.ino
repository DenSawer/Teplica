#include <WiFi.h>
#include <CRC32.h>
char espID[7];


uint8_t mac[6];
const char base62chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

// Получаем MAC-адрес с WiFi
void getMAC() {
  String macStr = WiFi.macAddress();
  for (int i = 0; i < 6; i++) {
    mac[i] = strtoul(macStr.substring(i * 3, i * 3 + 2).c_str(), nullptr, 16);
  }

  // CRC32 от MAC
  CRC32 crc;
  crc.update(mac, 6);
  uint32_t hash = crc.finalize();

  // Преобразуем в Base62 и записываем в espID (ровно 6 символов)
  for (int i = 5; i >= 0; i--) {
    espID[i] = base62chars[hash % 62];
    hash /= 62;
  }
  espID[6] = '\0';  // Завершающий символ строки
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(200);

  getMAC();
  Serial.print(espID);
}

void loop() {
  Serial.println(espID);
  Serial.println(int(espID));
  delay(2000);
}
