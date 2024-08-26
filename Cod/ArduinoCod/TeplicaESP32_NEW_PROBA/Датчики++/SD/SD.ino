#include <SPI.h>
#include <SD.h>

// Определение пинов для HSPI
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_CLK 14
#define HSPI_CS 15

// Объект HSPI
SPIClass hspi(HSPI);

void setup() {
  Serial.begin(115200);
  delay(5000);
  // Инициализация HSPI интерфейса
  hspi.begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, HSPI_CS);

  // Частота работы контроллера
  if (!SD.begin(HSPI_CS, hspi, 8000000)) {
    Serial.println("Ошибка монтирования карты");
    return;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("SD карта не обнаружена");
    return;
  }

  Serial.print("Тип SD карты: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("НЕИЗВЕСТНО");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Размер SD карты: %lluMB\n", cardSize);

  // Тестовые операции с файлами и директориями
  writeFile(SD, "/hello.txt", "Привет, Мир!\n");
  readFile(SD, "/hello.txt");
}

void loop() {
  // Основной цикл программы с небольшой задержкой
  delay(1000);
}

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
