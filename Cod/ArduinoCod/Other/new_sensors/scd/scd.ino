#include <Wire.h>
#include <SensirionI2cScd4x.h>
#include <Adafruit_SCD30.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27  // Адресс дисплея
#define LCD_COLUMNS 20    // Кол-во столбцов дисплея
#define LCD_ROWS 4        // Кол-во строк дисплея
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

SensirionI2cScd4x scd4x;
Adafruit_SCD30 scd30;
bool isSCD30 = false;
bool isSCD4x = false;

// Функция обнаружения датчика SCD30 или SCD4x
void detectSensor() {
  Wire.begin();
  delay(1000);

  // Проверка наличия SCD30
  if (scd30.begin()) {
    isSCD30 = true;
    Serial.println("Detected SCD30 sensor.");
    return;
  }

  // Проверка наличия SCD4x (SCD40/SCD41)
  scd4x.begin(Wire, SCD41_I2C_ADDR_62);
  uint64_t serialNumber = 0;
  if (scd4x.getSerialNumber(serialNumber) == 0) {
    isSCD4x = true;
    Serial.print("Detected SCD4x sensor. Serial: 0x");
    Serial.println((uint32_t)(serialNumber & 0xFFFFFFFF), HEX);
  } else {
    Serial.println("No compatible SCD sensor detected.");
  }
}

// Функция чтения данных с датчика
void readSensorData() {
  if (isSCD30) {
    if (scd30.dataReady()) {
      scd30.read();
      Serial.print("SCD30 - CO2: ");
      Serial.print(scd30.CO2, 3);
      Serial.print(" ppm, Temp: ");
      Serial.print(scd30.temperature, 3);
      Serial.print(" C, Humidity: ");
      Serial.print(scd30.relative_humidity, 3);
      Serial.println(" %");
      lcd.setCursor(0, 0);
      lcd.print("CO2: ");
      lcd.print(scd30.CO2, 3);
      lcd.print(" ppm");
      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.print(scd30.temperature, 3);
      lcd.print(" C");
      lcd.setCursor(0, 2);
      lcd.print("Humidity: ");
      lcd.print(scd30.relative_humidity, 3);
      lcd.print(" %");
    }
  } else if (isSCD4x) {
    uint16_t co2;
    float temp, humidity;
    if (scd4x.measureAndReadSingleShot(co2, temp, humidity) == 0) {
      Serial.print("SCD4x - CO2: ");
      Serial.print(co2);
      Serial.print(" ppm, Temp: ");
      Serial.print(temp);
      Serial.print(" C, Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    } else {
      Serial.println("Failed to read data from SCD4x.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  detectSensor();  // Определяем датчик
}

void loop() {
  readSensorData();  // Читаем данные с датчика
  delay(2000);       // Задержка между измерениями
}
