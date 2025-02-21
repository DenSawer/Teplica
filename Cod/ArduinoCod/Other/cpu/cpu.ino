#include "esp_timer.h"
#include <DHT.h>                // DHT
#define HEAT_PIN 32           // Пин для управления подогревом (красный светодиод)
#define COOL_PIN 33           // Пин для управления охлаждением (синий светодиод)
#define DHTTYPE DHT22  // Указываем используемый DHT
DHT dht(15, DHTTYPE);

uint64_t lastTime = 0;
uint64_t busyTime = 0;
uint64_t totalTime = 0;

void setup() {
  Serial.begin(115200);
  lastTime = esp_timer_get_time();  // Запоминаем время старта
}

void loop() {
  uint64_t start = esp_timer_get_time();  // Засекаем начало выполнения loop()

  // --- Здесь твой основной код ---
  // Чтение температуры и влажности с DHT22
  float temperature = dht.readTemperature();  // Чтение температуры
  float humidity = dht.readHumidity();        // Чтение влажности

  bool isCool = temperature < 28.0;  // холодно если меньше 23
  bool isHeat = temperature > 30.0;  // жарко если больше 24

  // Проверка, удалось ли прочитать данные с датчика DHT22
  if (!isnan(humidity) && !isnan(temperature)) {
    // Управление подогревом
    if (isCool) digitalWrite(HEAT_PIN, HIGH);  // Включить подогрев
    else digitalWrite(HEAT_PIN, LOW);          // Выключить подогрев
    // Управление охлаждением
    if (isHeat) digitalWrite(COOL_PIN, HIGH);  // Включить охлаждение
    else digitalWrite(COOL_PIN, LOW);          // Выключить охлаждение
  } else {
    Serial.println(F("Ошибка чтения данных с DHT22!"));  // Вывод сообщения об ошибке, если не удалось прочитать данные
    delay(500);                                          // Задержка для отображения сообщения
    return;
  }
  delay(1000);  // Например, работа кода
  // --------------------------------

  uint64_t end = esp_timer_get_time();  // Засекаем конец выполнения loop()

  busyTime += (end - start);  // Суммируем активное время
  totalTime += (end - lastTime);
  lastTime = end;

  if (totalTime >= 1000000) {  // Каждую секунду вычисляем загрузку
    float cpuLoad = (float)busyTime / (float)totalTime * 100.0;
    Serial.printf("🔹 Загруженность CPU: %.2f%%\n", cpuLoad);
    busyTime = 0;  // Сбрасываем счётчики
    totalTime = 0;
  }
}
