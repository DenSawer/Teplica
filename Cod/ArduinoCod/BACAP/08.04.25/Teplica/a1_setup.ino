void setup() {
  Serial.begin(115200);                            // Инициализация USB порта со скоростью 115200 бод
  sim800.begin(9600, SERIAL_8N1, SIM_RX, SIM_TX);  // Инициализация SIM порта со скоростью 9600 бод

  Wire.begin();  // I2C
  //--------------------------------Инициализация система---------------------------------------
  Serial.println("Начало инициализации системы...");
  initLCD();            // Инициализация LCD
  initSD();             // Инициализация SD
  loadSettings();       // Загрузка настроек
  initSIM();            // Инициализация SIM
  initEthernet();       // Инициализация Ethernet
  initWiFi();           // Инициализация WiFi, включение точки доступа и веб-сервера
  connectToInternet();  // Первичное подключение к Интернету по различным интерфейсам
  // ==== Sensors ====
  initCO2Sensor();
  initAirSensor();  // Инициализация DHT22
  initSoilMois();
  initSoilTemp();
  initLightSensors();  // Инициализация Датчиков освещённости

  // Сообщение о завершении инициализации системы
  Serial.println("Инициализация системы завершена.");
}

//Подсчет устройств для инициализации
#define allDevice 10.0  // Все датчики
uint8_t device = 0;

void showLoadingProgressBar(const char *str, bool yes_no) {
  lcd.clear();
  device++;
  // Подсчет и прорисовка загрузки
  uint8_t progressProcent = device / allDevice * 100;
  uint8_t progressBar = (progressProcent / 100.0) * LCD_COLUMNS;
  lcd.setCursor(0, 2);
  for (uint8_t i = 1; i < LCD_COLUMNS; i++) {
    if (i < progressBar) lcd.write(255);  // Полный блок █
  }
  lcd.setCursor(progressBar - 2, 3);
  lcd.print(progressProcent);
  lcd.print("%");
  
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.print(' ');
  Serial.print(str);
  if (yes_no) {
    lcd.write(SYM_GALOCHKA);  // ✔
    Serial.println(" Инициализация завершена.");
    delay(500);
  } else {
    Serial.println(" Отсутствует!");
    lcd.write(0x78);             // ✖
    tone(BIPER_PIN, 1000, 500);  // Звуковой сигнал ошибки
    tone(BIPER_PIN, 0, 200);
    tone(BIPER_PIN, 1000, 500);
    delay(3500);
  }
  if (device == allDevice) lcd.clear();
}
