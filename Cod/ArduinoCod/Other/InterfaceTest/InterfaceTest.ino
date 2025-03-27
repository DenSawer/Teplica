/*#include <Wire.h>  // Для I2C
#include <SPI.h>   // Для SPI

#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_CLK 18

#define I2C_SDA 21
#define I2C_SCL 22

#define TEST_PIN 2  // Тестируемый GPIO

const int spi_cs_pins[] = { 5, 4, 15, 2, 14, 13, 12 };  // Возможные CS пины
const int num_cs_pins = sizeof(spi_cs_pins) / sizeof(spi_cs_pins[0]);

void scanI2C() {
  Serial.println("\nI2C устройства:");
  Wire.begin(I2C_SDA, I2C_SCL);
  bool found = false;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("0x%02X ", address);
      found = true;
    }
  }
  if (!found) Serial.println("Нет устройств");
  else Serial.println();
}

void testSPI() {
  Serial.println("\nТест SPI...");
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI);
  bool found = false;
  for (int i = 0; i < num_cs_pins; i++) {
    pinMode(spi_cs_pins[i], OUTPUT);
    digitalWrite(spi_cs_pins[i], LOW);
    uint8_t response = SPI.transfer(0xAA);
    digitalWrite(spi_cs_pins[i], HIGH);
    if (response != 0x00 && response != 0xFF) {
      Serial.printf("SPI устройство найдено на CS %d, отклик: 0x%02X\n", spi_cs_pins[i], response);
      found = true;
    }
  }
  if (!found) Serial.println("Нет SPI устройств");
}

void testGPIO() {
  Serial.println("\nТест GPIO...");
  pinMode(TEST_PIN, OUTPUT);
  digitalWrite(TEST_PIN, HIGH);
  delay(500);
  digitalWrite(TEST_PIN, LOW);
  Serial.println("GPIO ОК");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nТест интерфейсов ESP32...");
  scanI2C();
  testSPI();
  testGPIO();
  Serial.println("\nТест завершен.");
}

void loop() {
  delay(5000);
}*/
