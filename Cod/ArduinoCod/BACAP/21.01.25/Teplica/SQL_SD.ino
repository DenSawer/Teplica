void openSQL() {
  // Открытие базы данных на SD-карте
  if (sqlite3_open("/sd/data.db", &db) == SQLITE_OK) {
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

void readSQL() {
  // Запись данных в базу данных
  char insertSQL[200];
  snprintf(insertSQL, sizeof(insertSQL),
           "INSERT INTO SensorData (temperature, humidity, light_level) VALUES (%.2f, %.2f, %d);",
           microclimate.temperature, microclimate.humidity, lightLevel);

  char *errMsg;
  if (sqlite3_exec(db, insertSQL, 0, 0, &errMsg) == SQLITE_OK) {
    Serial.println("Данные успешно записаны в таблицу");
  } else {
    Serial.print("Ошибка записи данных в таблицу: ");
    Serial.println(errMsg);
    sqlite3_free(errMsg);
  }

  // Задержка между циклами
  delay(10000);  // Записываем данные каждые 10 секунд
}

void closeSQL() {
  // Закрытие базы данных при завершении работы
  if (db) {
    sqlite3_close(db);
  }
}