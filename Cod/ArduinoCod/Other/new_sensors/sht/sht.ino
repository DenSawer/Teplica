// универсальная для sht

#include <Wire.h>
#include <Adafruit_SHT4x.h>
#include <Adafruit_SHT31.h>
#include <SHT2x.h>  // Для SHT2x (например, SHT21)

Adafruit_SHT4x sht4;
Adafruit_SHT31 sht3;
SHT2x sht2;

bool sht4_present = false;
bool sht3_present = false;
bool sht2_present = false;

// Функция инициализации всех датчиков SHT
void initSHT() {
  Wire.begin();

  // Инициализация SHT4x
  if (sht4.begin()) {
    sensors_event_t humidity, temp_event;
    sht4.getEvent(&humidity, &temp_event);
    if (!isnan(temp_event.temperature) && !isnan(humidity.relative_humidity)) {
      sht4_present = true;
      Serial.println("SHT4x найден.");
    } else {
      Serial.println("Ошибка данных SHT4x!");
    }
  } else {
    Serial.println("SHT4x не найден!");
  }

  // Инициализация SHT3x (0x44 или 0x45)
  if (sht3.begin(0x44) || sht3.begin(0x45)) {
    float temp = sht3.readTemperature();
    float hum = sht3.readHumidity();
    if (!isnan(temp) && !isnan(hum)) {
      sht3_present = true;
      Serial.println("SHT3x найден.");
    } else {
      Serial.println("Ошибка данных SHT3x!");
    }
  } else {
    Serial.println("SHT3x не найден!");
  }

  // Инициализация SHT2x
  if (sht2.begin()) {
    float temp = sht2.getTemperature();
    float hum = sht2.getHumidity();
    if (!isnan(temp) && !isnan(hum)) {
      sht2_present = true;
      Serial.println("SHT2x найден.");
    } else {
      Serial.println("Ошибка данных SHT2x!");
    }
  } else {
    Serial.println("SHT2x не найден!");
  }

  // Проверяем, найден ли хотя бы один датчик
  if (!sht4_present && !sht3_present && !sht2_present) {
    Serial.println("Ошибка: ни один SHT не найден!");
    while (1)
      ;  // Останавливаем выполнение
  }
}

// Функция считывания данных с датчиков
void readSHT(float &temp, float &hum) {
  if (sht4_present) {
    sensors_event_t humidity, temp_event;
    sht4.getEvent(&humidity, &temp_event);
    temp = temp_event.temperature;
    hum = humidity.relative_humidity;
  } else if (sht3_present) {
    temp = sht3.readTemperature();
    hum = sht3.readHumidity();
  } else if (sht2_present) {
    temp = sht2.getTemperature();
    hum = sht2.getHumidity();
  } else {
    temp = hum = -1;  // Ошибка чтения
  }
}

void setup() {
  Serial.begin(115200);
  initSHT();  // Инициализируем датчики
}

void loop() {
  float temperature, humidity;
  readSHT(temperature, humidity);

  Serial.printf("Температура: %.2f°C, Влажность: %.2f%%\n", temperature, humidity);
  Serial.println("--------------------------");
  delay(2000);  // Задержка 2 секунды
}
