#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 14  // Используемый пин для DS18B20

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
bool ds18b20_present = false;
int sensorCount = 0;

// Функция инициализации DS18B20
void initDS18B20() {
  sensors.begin();
  sensorCount = sensors.getDeviceCount();
  if (sensorCount > 0) {
    ds18b20_present = true;
    Serial.print("Найдено датчиков DS18B20: ");
    Serial.println(sensorCount);
  } else {
    ds18b20_present = false;
    Serial.println("Ошибка: датчики DS18B20 не найдены!");
  }
}

// Функция вывода средней температуры с датчиков
void readDS18B20() {
  if (ds18b20_present) {
    sensors.requestTemperatures();
    float sumTemp = 0;
    for (int i = 0; i < sensorCount; i++) {
      float temp = sensors.getTempCByIndex(i);
      Serial.print("Датчик ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(temp);
      Serial.println(" °C");
      sumTemp += temp;
    }
    float avgTemp = sumTemp / sensorCount;
    Serial.print("Средняя температура: ");
    Serial.print(avgTemp);
    Serial.println(" °C");
  } else {
    Serial.println("Ошибка: DS18B20 недоступен");
  }
}

void setup() {
  Serial.begin(115200);
  initDS18B20();
}

void loop() {
  readDS18B20();
  delay(1000);  // Задержка 1 секунда
}
