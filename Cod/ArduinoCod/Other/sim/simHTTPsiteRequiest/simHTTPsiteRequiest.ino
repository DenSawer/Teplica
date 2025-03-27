#define TINY_GSM_MODEM_SIM800  // Указываем модель модема
#include <TinyGsmClient.h>
#include <HardwareSerial.h>

#define MODEM_RX 16  // Пины подключения к SIM800L
#define MODEM_TX 17

const char* apn = "Inter96";       // APN
const char* gprsUser = "Inter96";  // Имя пользователя
const char* gprsPass = "Inter96";  // Пароль

HardwareSerial sim800(1);
TinyGsm modem(sim800);
TinyGsmClient client(modem);

void sendHttpRequest(const char* host, const char* path) {
  Serial.print("Подключение к ");
  Serial.println(host);

  if (!client.connect(host, 80)) {
    Serial.println("Ошибка подключения!");
    return;
  }

  Serial.println("Отправка HTTP-запроса...");
  client.print("GET ");
  client.print(path);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  }

  client.stop();
  Serial.println("\nСоединение закрыто.\n");
}

void setup() {
  Serial.begin(115200);
  sim800.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  Serial.println("Инициализация модема...");
  if (!modem.restart()) {
    Serial.println("Не удалось перезапустить модем!");
    return;
  }

  Serial.println("Подключение к сети...");
  if (!modem.waitForNetwork()) {
    Serial.println("Сеть не найдена!");
    return;
  }

  Serial.println("Подключение к GPRS...");
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("Ошибка GPRS!");
    return;
  }

  Serial.println("GPRS подключен!");
  Serial.print("IP-адрес: ");
  Serial.println(modem.localIP());
}

void loop() { 
  sendHttpRequest("example.com", "/");  // Запрос главной страницы example.com
  delay(5000);
  sendHttpRequest("jsonplaceholder.typicode.com", "/todos/1");  // Запрос JSON-данных
  delay(5000);
  Serial.println(sim800.print("AT+CCLK?"));
  //sendHttpRequest("api.timezonedb.com", "/v2.1/get-time-zone?key=YOUR_API_KEY&format=json&by=zone&zone=Europe/Moscow");
  delay(10000);  // Запросы раз в минуту
}
