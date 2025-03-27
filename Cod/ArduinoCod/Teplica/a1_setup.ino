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
  initDHT();            // Инициализация DHT22
  initLightSensor();    // Инициализация Датчиков освещённости
  initSoilMoisture();   // Инициализация Датчиков влажности почвы

  // Сообщение о завершении инициализации системы
  Serial.println("Инициализация системы завершена.");
}

//Подсчет устройств для инициализации
#define allDevice 8  // Все датчики
uint8_t device = 0;

void showLoadingProgressBar(const char *str, bool yes_no = false) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.print(' ');
  Serial.print(str);
  if (yes_no) {
    lcd.write(SYM_GALOCHKA);  // ✔
    Serial.println(" Инициализация завершена.");
  } else {
    Serial.println(" Отсутствует!");
    lcd.write(0x78);             // ✖
    tone(BIPER_PIN, 1000, 500);  // Звуковой сигнал ошибки
    tone(BIPER_PIN, 0, 200);
    tone(BIPER_PIN, 1000, 500);
    delay(3500);
  }
  device++;
  // Подсчет и прорисовка загрузки
  uint8_t progressProcent = device / allDevice * 100;
  uint8_t progressBar = (progressProcent / 100) * LCD_COLUMNS;
  lcd.setCursor(0, 2);
  for (uint8_t i = 1; i < LCD_COLUMNS; i++) {
    if (i < progressBar) lcd.write(255);  // Полный блок █
  }
  lcd.setCursor(0, 3);
  lcd.print(progressProcent);
  lcd.print("%");
  delay(500);
  if (progressBar == LCD_COLUMNS) lcd.clear();
}

void initLCD() {
  // Проверка наличия и автоматическое определение LCD дисплея
  if (!Wire.requestFrom(LCD_ADDRESS, 1)) {
    isPresent.LCD = false;
    showLoadingProgressBar("Display", false);
    delay(1000);
    return;
  }

  loadCustomChars();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(200);
  isPresent.LCD = true;
  showLoadingProgressBar("Display", true);
}

// Функция инициализации SD-карты
void initSD() {
  uint32_t start = millis();
  while (millis() - start < 3000 && !isPresent.SD) {
    if (SD.begin(SD_CS_PIN)) {
      isPresent.SD = true;
      showLoadingProgressBar("SD", true);
      return;
    }
  }
  isPresent.SD = false;
  showLoadingProgressBar("SD", false);
}

// Функция инициализации SIM-карты
void initSIM() {
  sim800.println("AT");
  uint32_t start = millis();
  while (millis() - start < 3000) {
    if (sim800.find("OK")) {
      isPresent.SIM = true;
      showLoadingProgressBar("SIM", true);
      return;
    }
  }
  isPresent.SIM = false;
  showLoadingProgressBar("SIM", false);
  if (!checkSIMCard()) {
    Serial.println("SIM-карта не найдена");
    return;
  }
  if (!connectGPRS()) {
    Serial.println("Ошибка подключения к GPRS");
  } else {
    Serial.println("GPRS подключен");
  }
}

// Функция проверки наличия SIM-карты
bool checkSIMCard() {
  if (!isPresent.SIM) return false;
  sim800.println("AT+CPIN?");
  uint32_t start = millis();
  while (millis() - start < 3000) {
    if (sim800.find("READY")) {
      return true;
    }
  }
  return false;
}

// Функция проверки наличия Ethernet модуля - ENC28J60
void initEthernet() {
  Ethernet.init(ETHERNET_CS_PIN);  // Инициализация ENC28J60 с указанием CS-пина
  static uint32_t start = millis();
  bool Eth = Ethernet.hardwareStatus() != EthernetNoHardware;
  while (millis() - start < 3000 && Eth)
    ;
  isPresent.Ethernet = Eth;
  showLoadingProgressBar("Ethernet", Eth);
}

// Функция инициализации Wi-Fi
void initWiFi() {
  WiFi.mode(WIFI_AP_STA);         // Режим работы и точки доступа и приема вифи
  WiFi.softAP(settings.ssid_AP);  // Имя точки доступа
  Serial.println("Точка доступа запущена");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());

  server.onNotFound(handleRequest);  // Запуск веб-интерфейса
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());  // DNS хрень которая должна принудительно открывать наш веб-интерфейс
  showLoadingProgressBar("Wi-Fi", true);
}

void initDHT() {
  pinMode(HEAT_PIN, OUTPUT);  // Пин для красного светодиода в режиме выхода
  pinMode(COOL_PIN, OUTPUT);  // Пин для синего светодиода в режиме выхода

  dht.begin();                           // Запуск работы с датчиком DHT
  data.airTemp = dht.readTemperature();  // Чтение температуры
  data.airHum = dht.readHumidity();      // Чтение влажности

  if (isnan(data.airTemp) || isnan(data.airHum)) {
    isPresent.airSensor = false;
    showLoadingProgressBar("DHT", false);
  } else {
    isPresent.airSensor = true;
    showLoadingProgressBar("DHT", true);
  }
}

void initLightSensor() {
  pinMode(LDR_PIN, INPUT);    // Пин для фоторезистора в режиме входа
  pinMode(LAMP_PIN, OUTPUT);  // Пин для лампы в режиме выхода

  digitalWrite(LAMP_PIN, LOW);  // Выключаем
  isPresent.lightSensor = true;
  showLoadingProgressBar("Light Sensor", true);
}

void initSoilMoisture() {
  pinMode(SOIL_MOISTURE_PIN, INPUT);  // Пин для датчика влажности почвы в режиме входа

  isPresent.soilMois = true;
  showLoadingProgressBar("Soil moisture", true);
}
