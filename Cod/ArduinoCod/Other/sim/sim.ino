#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <HardwareSerial.h>

#define MODEM_RX 16
#define MODEM_TX 17

const char apn[] = "Inter96";
const char gprsUser[] = "Inter96";
const char gprsPass[] = "Inter96";

HardwareSerial sim800(1);
TinyGsm modem(sim800);
TinyGsmClient client(modem);

// Функция получения времени с worldtimeapi.org
bool getTimeFromWorldTimeAPI(char* dateTimeBuffer, size_t bufferSize) {
  Serial.println("Попытка получения времени с worldtimeapi.org...");

  if (!client.connect("162.159.200.1", 80)) {
    Serial.println("Ошибка подключения (worldtimeapi)");
    return false;
  }

  client.println("GET /api/ip HTTP/1.1");
  client.println("Host: worldtimeapi.org");
  client.println("Connection: close");
  client.println();

  char payload[300] = { 0 };
  int index = 0;
  unsigned long timeout = millis();

  while (millis() - timeout < 5000) {
    while (client.available()) {
      char c = client.read();
      if (index < sizeof(payload) - 1) {
        payload[index++] = c;
      }
      timeout = millis();
    }
  }
  client.stop();
  payload[index] = '\0';

  char* dateTimeStart = strstr(payload, "\"datetime\":\"");
  if (!dateTimeStart) {
    Serial.println("Ошибка парсинга времени (worldtimeapi)");
    return false;
  }

  dateTimeStart += 11;
  char* dateTimeEnd = strchr(dateTimeStart, '"');
  if (!dateTimeEnd) {
    Serial.println("Ошибка формата времени (worldtimeapi)");
    return false;
  }

  size_t length = dateTimeEnd - dateTimeStart;
  if (length >= bufferSize) length = bufferSize - 1;

  strncpy(dateTimeBuffer, dateTimeStart, length);
  dateTimeBuffer[length] = '\0';

  Serial.print("Время (worldtimeapi): ");
  Serial.println(dateTimeBuffer);
  return true;
}

// Подключение к GPRS (вернул старый рабочий порядок)
bool connectGPRS() {
  Serial.println("Подключение к GPRS...");

  if (modem.isGprsConnected()) {
    Serial.println("GPRS уже подключен.");
    return true;
  }

  modem.gprsDisconnect();
  delay(2000);

  if (modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("GPRS подключен!");
    return true;
  } else {
    Serial.println("Ошибка GPRS!");
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);  // Скорость 9600 для стабильности

  Serial.println("Инициализация модема...");
  if (!modem.restart()) {
    Serial.println("Ошибка: не удалось перезапустить модем!");
    return;
  }

  Serial.println("Ожидание сети...");
  if (!modem.waitForNetwork(60000)) {
    Serial.println("Ошибка: сеть не найдена!");
    return;
  }

  connectGPRS();
}

void loop() {
  if (!modem.isGprsConnected()) {
    Serial.println("GPRS отключен, переподключаем...");
    connectGPRS();
    delay(5000);
  }

  char currentTime[30] = { 0 };

  if (!getTimeFromWorldTimeAPI(currentTime, sizeof(currentTime))) {
    Serial.println("WorldTimeAPI не отвечает.");
  }

  delay(60000);
}
