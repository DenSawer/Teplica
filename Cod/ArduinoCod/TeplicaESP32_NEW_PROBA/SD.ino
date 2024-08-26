/*
  // Тестовые операции с файлами и директориями
  writeFile(SD, "/hello.txt", "Привет, Мир!\n");
  readFile(SD, "/hello.txt");
*/
void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Запись в файл: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Не удалось открыть файл для записи");
    return;
  }
  if (file.print(message)) {
    Serial.println("Файл записан");
  } else {
    Serial.println("Ошибка записи");
  }
  file.close();
}

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Чтение файла: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Не удалось открыть файл для чтения");
    return;
  }

  Serial.print("Содержимое файла: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}
