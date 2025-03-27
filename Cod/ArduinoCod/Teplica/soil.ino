#define SOIL_SENSOR_PIN 35  // Подключите аналоговый вывод датчика к GPIO34 (VP)

bool soilSensorPresent = true;

void initSoilSensor() {
  pinMode(SOIL_SENSOR_PIN, INPUT);
  soilSensorPresent = true;
  Serial.println("Датчик влажности найден.");
}

void readSoilMoisture() {
  if (soilSensorPresent) {
    uint16_t moisture = analogRead(SOIL_SENSOR_PIN);
    Serial.print("Уровень влажности почвы: ");
    Serial.println(moisture);
  }
}

void setup() {
  Serial.begin(115200);
  initSoilSensor();
}

void loop() {
  readSoilMoisture();
  delay(1000);  // Ожидание перед следующим измерением
}
