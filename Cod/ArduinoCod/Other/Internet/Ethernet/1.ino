#include <UIPEthernet.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <WiFi.h>  // Нужен для работы HTTPClient

#define ENC28J60_CS 5  // Определяем CS на 5-м порту

EthernetClient client;

// Функция генерации случайного MAC-адреса
void generateRandomMAC(uint8_t *mac) {
  mac[0] = 0x02;  // Локально администрируемый MAC (02:xx:xx:xx:xx:xx)
  for (int8_t i = 1; i < 6; i++) {
    mac[i] = random(0, 256);
  }
}

// Функция проверки наличия ENC28J60
bool isENC28J60Available() {
  Ethernet.init(ENC28J60_CS);  // Инициализация ENC28J60 с указанием CS-пина
  static uint32_t start = millis();
  while (millis() - start < 3000 && Ethernet.hardwareStatus() != EthernetNoHardware)
    ;
  return Ethernet.hardwareStatus() != EthernetNoHardware;
}

// Функция инициализации ENC28J60 и получения IP через DHCP
bool connectToEthernet() {
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

void setup() {
  Serial.begin(115200);  // Запускаем Serial-порт
  delay(1000);

  if (!isENC28J60Available()) {
    Serial.println("Ошибка: модуль ENC28J60 не найден!");
    return;
  }

  if (!connectToEthernet()) {
    Serial.println("Ошибка инициализации Ethernet!");
    return;
  }

  String response = httpRequest("http://example.com");
  Serial.println("Ответ сервера: " + response);
}

void loop() {
  // Можно делать периодические HTTP-запросы или выполнять другие задачи
}