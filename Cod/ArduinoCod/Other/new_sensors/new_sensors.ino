#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <Adafruit_SHT31.h>

// Объекты датчиков
Adafruit_BME280 bme;
BH1750 lightMeter;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

bool bme_present = false;
bool bh1750_present = false;
bool sht31_present = false;

// Функция инициализации BME280
void initBME280() {
  if (!bme.begin(0x76)) {
    Serial.println("BME280 не найден!");
    bme_present = false;
    return;
  }
  if (isnan(bme.readTemperature()) || isnan(bme.readHumidity())) {
    bme_present = false;
    Serial.println("BME280 не найден!");
  } else {
    bme_present = true;
    Serial.println("BME280 найден.");
  }
}

// Функция считывания данных с BME280
void readBME280() {
  if (bme_present) {
    Serial.print("BME280: T=");
    Serial.print(bme.readTemperature());
    Serial.print("C H=");
    Serial.print(int(bme.readHumidity()));
    Serial.println("%");
  }
}

// Функция инициализации BH1750
void initBH1750() {
  if (!lightMeter.begin() && !Wire.requestFrom(0x23, 1)) {
    bh1750_present = false;
    Serial.println("BH1750 не найден!");
    return;
  }
  if (isnan(lightMeter.readLightLevel())) {
    Serial.println("BH1750 не найден!");
    bh1750_present = false;
  } else {
    Serial.println("BH1750 найден.");
    bh1750_present = true;
  }
}

// Функция считывания данных с BH1750
void readBH1750() {
  if (bh1750_present) {
    Serial.print("BH1750 Lux: ");
    Serial.println(int(lightMeter.readLightLevel()));
  }
}

// Функция инициализации SHT31
void initSHT31() {
  if (!sht31.begin(0x44)) {
    sht31_present = false;
    Serial.println("SHT31 не найден!");
    return;
  }

  if (isnan(sht31.readTemperature()) || isnan(sht31.readHumidity())) {
    sht31_present = false;
    Serial.println("SHT31 не найден!");
  } else {
    sht31_present = true;
    Serial.println("SHT31 найден.");
  }
}

// Функция считывания данных с SHT31
void readSHT31() {
  if (sht31_present) {
    Serial.print("SHT31: T=");
    Serial.print(sht31.readTemperature());
    Serial.print("C H=");
    Serial.println(sht31.readHumidity());
  }
}



void setup() {
  Serial.begin(115200);
  Wire.begin();

  initBME280();
  initBH1750();
  initDS18B20();
  initSHT31();
  initSCD30();
}

void loop() {
  readBME280();
  readBH1750();
  readDS18B20();
  readSHT31();
  readSCD30();
  readSoilMoisture();
  delay(2000);
}
