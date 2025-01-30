#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <HardwareSerial.h>

#define RX_PIN 16  
#define TX_PIN 17  
HardwareSerial SerialAT(2);

TinyGsm modem(SerialAT);

// Ваши данные APN, пользователя и пароля
const char* apn = "Inter96";       // APN
const char* gprsUser = "Inter96";  // Имя пользователя
const char* gprsPass = "Inter96";  // Пароль

void setup() {
  Serial.begin(115200);
  SerialAT.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(3000);

  Serial.println("Проверка связи с модемом...");
  SerialAT.println("AT");
  delay(1000);
  while (SerialAT.available()) Serial.write(SerialAT.read());

  Serial.println("\nПроверка регистрации в сети...");
  SerialAT.println("AT+CREG?");
  delay(2000);
  while (SerialAT.available()) Serial.write(SerialAT.read());

  Serial.println("\nПроверка уровня сигнала...");
  SerialAT.println("AT+CSQ");
  delay(2000);
  while (SerialAT.available()) Serial.write(SerialAT.read());

  Serial.println("\nНастройка APN...");
  SerialAT.print("AT+CSTT=\"");  
  SerialAT.print(apn);          // Устанавливаем APN
  SerialAT.print("\",\"");
  SerialAT.print(gprsUser);     // Устанавливаем имя пользователя
  SerialAT.print("\",\"");
  SerialAT.print(gprsPass);     // Устанавливаем пароль
  SerialAT.println("\"");
  delay(2000);
  while (SerialAT.available()) Serial.write(SerialAT.read());

  Serial.println("\nЗапуск GPRS...");
  SerialAT.println("AT+CIICR");
  delay(5000);
  while (SerialAT.available()) Serial.write(SerialAT.read());

  Serial.println("\nПолучение IP-адреса...");
  SerialAT.println("AT+CIFSR");
  delay(2000);
  while (SerialAT.available()) Serial.write(SerialAT.read());

  Serial.println("\nОтправка HTTP-запроса...");
  SerialAT.println("AT+HTTPINIT");
  delay(1000);
  SerialAT.println("AT+HTTPPARA=\"CID\",1");
  delay(1000);
  SerialAT.println("AT+HTTPPARA=\"URL\",\"http://example.com\"");
  delay(1000);
  SerialAT.println("AT+HTTPACTION=0");  // 0 - GET-запрос
  delay(5000);

  while (SerialAT.available()) {
    Serial.write(SerialAT.read());
  }

  SerialAT.println("AT+HTTPTERM");  // Завершаем HTTP-сессию
}

void loop() {}
