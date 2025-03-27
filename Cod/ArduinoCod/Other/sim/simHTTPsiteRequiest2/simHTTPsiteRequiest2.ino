#include <HardwareSerial.h>

HardwareSerial sim800(1);
#define SIM_RX 16  // Пины подключения к sim800
#define SIM_TX 17
#define SIM_BAUD 9600

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
    if (sim800.find("OK")) break;
  }

  sim800.print("AT+CSTT=\"");
  sim800.print(apn);
  sim800.print("\",\"");
  sim800.print(user);
  sim800.print("\",\"");
  sim800.print(pass);
  sim800.println("\"");

  start = millis();
  while (millis() - start < 1000) {
    if (sim800.find("OK")) break;
  }

  sim800.println("AT+CGATT=1");
  start = millis();
  while (millis() - start < 2000) {
    if (sim800.find("OK")) break;
  }

  sim800.println("AT+CIICR");
  start = millis();
  while (millis() - start < 5000) {
    if (sim800.find("OK")) break;
  }

  sim800.println("AT+CIFSR");
  return sim800.find(".");
}

bool sendHTTPRequest(const char *host) {
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

void setup() {
  Serial.begin(115200);
  sim800.begin(SIM_BAUD, SERIAL_8N1, SIM_RX, SIM_TX);
  initSIM();
  if (!checkSIMCard()) {
    Serial.println("SIM-карта не найдена");
    return;
  }
  if (!connectGPRS()) {
    Serial.println("Ошибка подключения к GPRS");
  } else {
    Serial.println("GPRS подключен");
  }
}

void loop() {
  if (sendHTTPRequest("example.com")) {
    Serial.println("HTTP-запрос успешен");
  } else {
    Serial.println("Ошибка HTTP-запроса");
  }
  Serial.println(getMCCMNC());
  uint32_t lastRequest = millis();
  while (millis() - lastRequest < 5000) {}
}