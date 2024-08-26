#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#include <Wire.h>
#include <sqlite3.h>

// Настройки подключения к датчикам
#define DHTPIN 4           // Пин подключения DHT датчика
#define DHTTYPE DHT22      // Указываем тип используемого DHT датчика (в данном случае DHT22)
DHT dht(DHTPIN, DHTTYPE);  // Создаем объект для работы с DHT датчиком


// Настройки SD карты
#define SD_CS 15  // Пин чип селект для SD карты

// Объекты и переменные для работы с SQLite
sqlite3 *db;         // Указатель на объект базы данных SQLite
sqlite3_stmt *stmt;  // Указатель на объект SQL-запроса
char *zErrMsg = 0;   // Строка для хранения сообщения об ошибке SQL
int rc;              // Код возврата для проверки успешности выполнения SQL-команд

// Функция для открытия базы данных на SD-карте
void openDatabase() {
  // Инициализация SD-карты
  if (SD.begin(SD_CS)) {
    Serial.println("SD-карта готова к использованию");  // Если карта готова, выводим сообщение
  } else {
    Serial.println("Не удалось инициализировать SD-карту");  // В случае ошибки инициализации выводим сообщение
    return;
  }

  // Путь к базе данных на SD-карте
  String databasePath = "/data.db";  // Имя файла базы данных

  // Открытие базы данных или создание новой, если она не существует
  rc = sqlite3_open((SD.begin() + databasePath).c_str(), &db);
  if (rc) {
    Serial.print("Не удается открыть базу данных:");  // Если не удалось открыть базу данных, выводим сообщение
    Serial.println(sqlite3_errmsg(db));
    return;
  } else {
    Serial.println("База данных успешно открыта");  // Сообщение об успешном открытии базы данных
  }
}

// Функция для создания таблицы, если она еще не существует
void createTable() {
  // SQL-запрос для создания таблицы
  const char *sql = "CREATE TABLE IF NOT EXISTS sensor_data ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "temperature REAL,"
                    "humidity REAL,"
                    "light REAL,"
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

  // Выполнение SQL-запроса
  rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.print("SQL error: ");  // Если ошибка при создании таблицы, выводим сообщение
    Serial.println(zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    Serial.println("Table created successfully");  // Сообщение об успешном создании таблицы
  }
}

// Функция для вставки данных в таблицу
void insertData(float temperature, float humidity, float light) {
  // SQL-запрос для вставки данных
  const char *sql = "INSERT INTO sensor_data (temperature, humidity, light) VALUES (?, ?, ?);";

  // Подготовка SQL-запроса
  sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  // Привязка значений к параметрам SQL-запроса
  sqlite3_bind_double(stmt, 1, temperature);
  sqlite3_bind_double(stmt, 2, humidity);
  sqlite3_bind_double(stmt, 3, light);

  // Выполнение SQL-запроса
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    Serial.print("Не удалось вставить данные:");  // Сообщение об ошибке вставки данных
    Serial.println(sqlite3_errmsg(db));
  } else {
    Serial.println("Данные были успешно вставлены");  // Сообщение об успешной вставке данных
  }
  sqlite3_finalize(stmt);  // Завершение работы с подготовленным запросом
}

// Функция для чтения данных из таблицы
void readData() {
  // SQL-запрос для получения всех данных из таблицы
  const char *sql = "SELECT * FROM sensor_data;";

  // Подготовка SQL-запроса
  sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
  // Проход по всем строкам результата запроса
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    // Извлечение и вывод данных для каждой строки
    Serial.print("ID: ");
    Serial.println(sqlite3_column_int(stmt, 0));
    Serial.print("Temperature: ");
    Serial.println(sqlite3_column_double(stmt, 1));
    Serial.print("Humidity: ");
    Serial.println(sqlite3_column_double(stmt, 2));
    Serial.print("Light: ");
    Serial.println(sqlite3_column_double(stmt, 3));
    Serial.print("Timestamp: ");
    Serial.println((const char *)sqlite3_column_text(stmt, 4));
  }
  sqlite3_finalize(stmt);  // Завершение работы с подготовленным запросом
}

// Функция для удаления старых данных
void deleteOldData(int months) {
  // SQL-запрос для удаления данных старше определенного количества месяцев
  String sql = "DELETE FROM sensor_data WHERE timestamp < datetime('now', '-" + String(months) + " months');";

  // Выполнение SQL-запроса
  rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.print("SQL error: ");  // Сообщение об ошибке удаления данных
    Serial.println(zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    Serial.println("Старые данные успешно удалены");  // Сообщение об успешном удалении старых данных
  }
}

void setup() {
  Serial.begin(115200);  // Инициализация последовательного порта для вывода отладочной информации

  dht.begin();   // Инициализация DHT датчика
  Wire.begin();  // Инициализация I2C шины

  openDatabase();  // Открытие или создание базы данных на SD-карте
  createTable();   // Создание таблицы, если она еще не существует
}

void loop() {
  Serial.print("Памяти занято: ");
  Serial.print(ESP.getFlashChipSize());

  // Чтение данных с датчиков
  float temperature = dht.readTemperature();  // Чтение температуры с DHT датчика
  float humidity = dht.readHumidity();        // Чтение влажности с DHT датчика
  float lightLevel = 3.3;                     // Чтение уровня освещенности с BH1750

  if (isnan(temperature) || isnan(humidity)) {          // Проверка на корректность данных с DHT датчика
    Serial.println("Failed to read from DHT sensor!");  // Сообщение об ошибке чтения
    return;
  }

  // Вставка данных в таблицу
  insertData(temperature, humidity, lightLevel);

  // Удаление данных старше 6 месяцев (параметр можно изменить)
  //deleteOldData(6);

  // Чтение данных из таблицы и вывод в последовательный порт
  readData();

  // Задержка перед следующим чтением данных (60 секунд)
  delay(1000);  // Отправка данных каждую минуту
}
