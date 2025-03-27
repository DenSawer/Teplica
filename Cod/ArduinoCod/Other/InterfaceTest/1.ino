#include <Wire.h>

#define SDA_PIN 21  // Укажите правильные пины, если используете другие
#define SCL_PIN 22

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  // Запуск I2C с указанными пинами
  Wire.begin();
  Serial.println("\nЗапуск точного сканирования I2C...");

  int devicesFound = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {  // Если устройство ответило
      delay(5);        // Небольшая задержка для стабилизации
      Wire.beginTransmission(address);
      if (Wire.endTransmission() == 0) {  // Двойная проверка
        Serial.print("Найдено устройство: 0x");
        Serial.println(address, HEX);
        devicesFound++;
      }
    }
    delay(2);  // Минимальная задержка между проверками
  }

  if (devicesFound == 0) {
    Serial.println("Устройства не найдены. Проверьте соединения.");
  } else {
    Serial.print("Всего найдено устройств: ");
    Serial.println(devicesFound);
  }
}

void loop() {
  // Пусто, так как сканирование выполняется один раз
}
