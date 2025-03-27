/*#include <SPI.h>
#include <SD.h>
#include <WiFi.h>

#define HSPI_MISO 19  // HSPI
#define HSPI_MOSI 23  // HSPI
#define HSPI_CLK 18   // HSPI
#define SD_CS_PIN 4   // HSPI CS для SD

struct {
  bool SD = false;
} isPresent;

// Структура для хранения настроек
struct {
  String ssid;
  String password;
} settings;

// Функция инициализации SD-карты
void initSD() {
  uint32_t start = millis();
  while (millis() - start < 3000 && !isPresent.SD) {
    if (SD.begin(SD_CS_PIN)) {
      Serial.println("SD-карта инициализирована.");
      isPresent.SD = true;
      return;
    }
  }
  Serial.println("Ошибка инициализации SD-карты!");
  isPresent.SD = false;
}


void reloadDisableModules() {
  static uint64_t start = millis();
  if (millis() - start < 60000) {
    if (!isPresent.SD){
      initSD();
      start = millis();
    }
  }
}

void setup() {
  Serial.begin(115200);
  initSD();
  reloadDisableModules();
}

void loop() {
}*/