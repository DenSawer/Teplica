#define MODEM_RX 16  // Пины подключения к SIM800L
#define MODEM_TX 17

HardwareSerial SIM800L(1);

void setup() {
  // Инициализация сериал-порта для ESP32
  Serial.begin(115200);
  SIM800L.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);

  delay(1000);
  Serial.println("Initializing SIM800L...");
}

void loop() {
  // Отправляем команду на получение даты и времени
  SIM800L.println("AT+CCLK?");
  String response = SIM800L.readString();
  Serial.println(response);
}
