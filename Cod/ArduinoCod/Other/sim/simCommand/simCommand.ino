#define MODEM_RX 16  // Пины подключения к sim800
#define MODEM_TX 17

HardwareSerial sim800(1);

void sim(String command) {
  sim800.println(command);
  String response = sim800.readString();
  Serial.println(response);
  delay(1000);
}

void setup() {
  // Инициализация сериал-порта для ESP32
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);

  delay(1000);
  Serial.println("start");
  sim("AT+CSCS=\"GSM\"");  //GSM подключение
  sim("AT+COPS=0,0");      // вывод операторов в численном виде
}

void loop() {

  sim("AT+CIFSR");                         //	Получение локального IP-адреса.
  //sim("AT+CSTT?");                         //пишет заданный апн
  //sim("AT+COPS?");                         //оператор
  sim("AT+CSTT=Inter96,Inter96,Inter96");  //задаем апн
  sim("AT+CGDCONT=1,\"IP\",\"Inter96\"");  //задаем апн
  sim("AT+CGDCONT?");  //задаем апн
  sim("AT+CGATT=1");

  //sim("AT+CGATT");                         //Установка/разрыв GPRS соединения.
  //sim("AT+CGDATA");                        //Установка связи с сетью
  //sim("AT+CGCLASS");                       //Установка/запрос GPRS класса мобильного устройства.
  delay(1000);
}