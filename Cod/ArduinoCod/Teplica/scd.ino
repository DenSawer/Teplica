#include <Wire.h>
#include <Adafruit_SCD30.h>
#include <SensirionI2CScd4x.h>

Adafruit_SCD30 scd30;
SensirionI2CScd4x scd4x;
bool isSCD30 = false;
bool isSCD4x = false;

// Функция инициализации SCD30 или SCD4x
void initSCD() {
  Serial.println("Detecting SCD sensor...");

  // Проверка наличия датчика SCD30
  if (scd30.begin()) {
    isSCD30 = true;
    Serial.println("SCD30 detected!");
  } else {
    // Проверка наличия датчика SCD40/SCD41
    scd4x.begin(Wire);
    uint16_t error = scd4x.stopPeriodicMeasurement();  // Остановка периодических измерений для проверки связи
    delay(500);
    if (error == 0) {
      isSCD4x = true;
      Serial.println("SCD4x (SCD40/SCD41) detected!");
      scd4x.startPeriodicMeasurement();
    } else {
      Serial.println("No SCD sensor detected!");
    }
  }
}

// Функция считывания данных с SCD30 или SCD4x
void readSCD() {
  if (isSCD30) {
    if (scd30.dataReady()) {
      if (!scd30.read()) {
        Serial.println("Error reading SCD30 data");
        return;
      }
      // Вывод данных с SCD30
      Serial.print("CO2: ");
      Serial.print(scd30.CO2, 3);
      Serial.print(" ppm, Temp: ");
      Serial.print(scd30.temperature, 3);
      Serial.print(" C, Humidity: ");
      Serial.print(scd30.relative_humidity, 3);
      Serial.println(" %");
    }
  } else if (isSCD4x) {
    uint16_t co2;
    float temperature, humidity;
    uint16_t error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error == 0) {
      // Вывод данных с SCD4x
      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.print(" ppm, Temp: ");
      Serial.print(temperature);
      Serial.print(" C, Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    } else {
      Serial.println("Error reading SCD4x data");
    }
  }
}

void setup() {
  Serial.begin(115200);  // Инициализация последовательного соединения
  Wire.begin();          // Инициализация I2C
  delay(1000);           // Задержка для стабилизации датчиков
  initSCD();             // Инициализация датчиков
}

void loop() {
  readSCD();    // Чтение данных с датчиков
  delay(2000);  // Задержка перед следующим считыванием
}
