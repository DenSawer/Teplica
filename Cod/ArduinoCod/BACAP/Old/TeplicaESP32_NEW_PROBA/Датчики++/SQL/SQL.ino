#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#include <sqlite3.h>

// Определение пинов для HSPI
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_CLK 14
#define HSPI_CS 15

// Определение пинов для датчиков
#define DHTPIN 4         // Пин для DHT22
#define DHTTYPE DHT22    // Тип DHT датчика
#define LDRPIN 34        // Пин для аналогового фоторезистора

// Объекты для работы с датчиками
DHT dht(DHTPIN, DHTTYPE);
SPIClass hspi(HSPI);

// Указатель на базу данных
sqlite3 *db;

void setup() {
  Serial.begin(115200);
  delay(5000);

  // Инициализация датчика DHT22
  dht.begin();

  // Инициализация HSPI интерфейса и SD карты
  hspi.begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, HSPI_CS);

  if (!SD.begin(HSPI_CS, hspi, 8000000)) {
    Serial.println("Ошибка монтирования карты");
    return;
  }

  if (SD.cardType() == CARD_NONE) {
    Serial.println("SD карта не обнаружена");
    return;
  }

  // Открытие базы данных на SD-карте
  if (sqlite3_open("/sd/database/data.db", &db) == SQLITE_OK) {
    Serial.println("Подключение к базе данных успешно");

    // Создание таблицы для хранения данных
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS SensorData ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "temperature REAL, "
                                 "humidity REAL, "
                                 "light_level INTEGER, "
                                 "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)";
    char *errMsg;
    if (sqlite3_exec(db, createTableSQL, 0, 0, &errMsg) != SQLITE_OK) {
      Serial.print("Ошибка создания таблицы: ");
      Serial.println(errMsg);
      sqlite3_free(errMsg);
    } else {
      Serial.println("Таблица SensorData создана");
    }
  } else {
    Serial.println("Ошибка подключения к базе данных");
  }
}

void loop() {
  // Считывание данных с датчиков
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int lightLevel = analogRead(LDRPIN);

  // Проверка, что данные корректны
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Ошибка считывания данных с DHT22");
    return;
  }

  // Запись данных в базу данных
  char insertSQL[200];
  snprintf(insertSQL, sizeof(insertSQL), 
           "INSERT INTO SensorData (temperature, humidity, light_level) VALUES (%.2f, %.2f, %d);", 
           temperature, humidity, lightLevel);

  char *errMsg;
  if (sqlite3_exec(db, insertSQL, 0, 0, &errMsg) == SQLITE_OK) {
    Serial.println("Данные успешно записаны в таблицу");
  } else {
    Serial.print("Ошибка записи данных в таблицу: ");
    Serial.println(errMsg);
    sqlite3_free(errMsg);
  }

  // Задержка между циклами
  delay(10000); // Записываем данные каждые 10 секунд
}

void closeDB() {
  // Закрытие базы данных при завершении работы
  if (db) {
    sqlite3_close(db);
  }
}