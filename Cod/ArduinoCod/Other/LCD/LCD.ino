#include <Wire.h>

void scanI2C() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Начало сканирования I2C шины...");
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Найдено устройство по адресу 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
    // Небольшая задержка для стабильности
    delay(5);
  }

  if (nDevices == 0)
    Serial.println("Устройства не найдены.");
  else
    Serial.print("Найдено устройств: "), Serial.println(nDevices);
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial)
    ;  // Ждём инициализации Serial (для некоторых плат)
  scanI2C();
}

void loop() {
  // Можно запускать сканирование по кнопке или по таймеру, если необходимо
}
