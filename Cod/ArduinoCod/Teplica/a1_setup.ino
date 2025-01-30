void setup() {

  Serial.begin(115200);  // Инициализация серийного порта со скоростью 115200 бод

  //--------------------------------Инициализация система---------------------------------------
  Serial.println("Начало инициализации системы...");
  setup_DP();  // Инициализация LCD и 7seg disp
  //setup_SD();        // Инициализация SD
  setup_RTC();            // Инициализация RTC
  setup_Ethernet();       // Инициализация Ethernet
  setup_DHT();            // Инициализация DHT22
  setup_light();          // Инициализация Датчиков освещённости
  setup_soil_moisture();  // Инициализация Датчиков влажности почвы
  //openSQL();         // Обработка SQL

  // Сообщение о завершении инициализации системы
  Serial.println("Инициализация системы завершена.");
}

void showLoadingprogressBar(const char *str, bool yes_no = false) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.print(' ');
  Serial.print(str);
  Serial.print(' ');
  if (yes_no) {
    lcd.write(char(0));  // ✔
    Serial.println("Инициализация завершена.");
  } else {
    Serial.println("Отсутствует!");
    lcd.write(char(0x78));       // ✖
    tone(BIPER_PIN, 1000, 500);  // Звуковой сигнал ошибки
    tone(BIPER_PIN, 0, 200);
    tone(BIPER_PIN, 1000, 500);
    delay(3500);
  }
  device++;
  uint8_t progressBar = (device * LCD_COLUMN) / allDevice;
  lcd.setCursor(0, 1);
  for (uint8_t i = 0; i < progressBar; i++) {
    lcd.print(char(255));  // Полный блок █
  }
  delay(500);
  if (progressBar == LCD_COLUMN) {
    lcd.clear();
    return;
  }
}

void setup_DP() {
  // Проверка наличия и автоматическое определение LCD дисплея
  Wire.begin();
  if (!Wire.requestFrom(LCD_ADDRESS, 1)) {
    showLoadingprogressBar("Display", false);
    delay(1000);
    return;
  }

  byte galochkaChar[] = { 0x00, 0x00, 0x01, 0x02, 0x14, 0x08, 0x00, 0x00 };
  lcd.createChar(0, galochkaChar);
  byte gradusCChar[] = { 0x1c, 0x14, 0x1c, 0x03, 0x04, 0x04, 0x04, 0x03 };
  lcd.createChar(1, gradusCChar);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(200);
  showLoadingprogressBar("Display", true);
}

void setup_SD() {
  delay(1000);

  // Инициализация HSPI интерфейса
  hspi.begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, HSPI_CS);

  // Частота работы контроллера
  if (!SD.begin(HSPI_CS, hspi, 8000000)) {
    Serial.println("ОШИБКА монтирования карты памяти");
    showLoadingprogressBar("SD", true);
    return;
  }
  if (SD.cardType() == CARD_NONE) {
    Serial.println("SD карта не обнаружена");
    showLoadingprogressBar("SD", false);
    return;
  } else {
    Serial.print("SD карта на месте");
    showLoadingprogressBar("SD", true);
  }
}

void setup_RTC() {
  if (!rtc.begin()) {  // Проверка успешной инициализации RTC
    showLoadingprogressBar("RTC", false);
    return;
  }
  // Установка времени, если RTC не настроен
  if (!rtc.isrunning()) {                            // Проверка, работает ли RTC
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Установка времени по дате и времени компиляции
    showLoadingprogressBar("RTC time", true);
    Serial.println("Установка времени по компьютеру.");  // Сообщение об установке времени
  } else {
    showLoadingprogressBar("RTC", true);
  }
}

void setup_Ethernet() {
  /*Serial.println("Начало инициализации Ethernet...");
  Ethernet.init(ETHERNET_CS_PIN);  // Настройка для MKR ETH Shield (используется CS-пин 5)
  if (Ethernet.begin(mac) == 0) {  // Если не удалось получить IP через DHCP
    Serial.println("Ethernet: не удалось получить IP через DHCP, используется статический IP.");
    Ethernet.begin(mac, ip, dnsServer, gw, sn);  // Использование статического IP
    Serial.println("Ethernet: инициализация завершена.");
  } else {
    Serial.println("Ethernet: получен IP через DHCP.");
  }
  delay(1000);  // Задержка для завершения инициализации

  // Вывод информации о сетевых параметрах
  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());

  // Подключение к серверу по HTTP
  if (client.connect(server, 80)) {
    Serial.println("Подключено к серверу!");
    client.println("GET /get HTTP/1.1");  // Отправка HTTP GET-запроса
    client.println("Host: httpbin.org");  // Указание хоста
    client.println("Connection: close");  // Закрытие соединения после завершения запроса
    client.println();                     // Пустая строка для завершения заголовков запроса
    beginMicros = micros();               // Запоминание времени начала передачи данных
  } else {
    Serial.println("Ошибка подключения к серверу.");
  }*/
  /*Serial.println("Инициализация ENC28J60 на ESP32...");

  // Устанавливаем пин CS для ENC28J60
  Ethernet.init(ETHERNET_CS_PIN);

  // Инициализация Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Ошибка инициализации Ethernet. Проверьте подключение.");
    showLoadingprogressBar("Ethernet", false);
  }

  Serial.print("IP-адрес: ");
  Serial.println(Ethernet.localIP());*/

  showLoadingprogressBar("Ethernet", true);
}

void setup_DHT() {
  pinMode(HEAT_PIN, OUTPUT);  // Пин для красного светодиода в режиме выхода
  pinMode(COOL_PIN, OUTPUT);  // Пин для синего светодиода в режиме выхода

  dht.begin();                                       // Запуск работы с датчиком DHT
  microclimate.temperature = dht.readTemperature();  // Чтение температуры
  microclimate.humidity = dht.readHumidity();        // Чтение влажности

  if (isnan(microclimate.temperature) || isnan(microclimate.humidity)) {
    showLoadingprogressBar("DHT", false);
  } else {
    showLoadingprogressBar("DHT", true);
  }
}

void setup_light() {
  pinMode(LDR_PIN, INPUT);    // Пин для фоторезистора в режиме входа
  pinMode(LAMP_PIN, OUTPUT);  // Пин для лампы в режиме выхода

  digitalWrite(LAMP_PIN, LOW);  // Выключаем
  showLoadingprogressBar("Light", true);
}

void setup_soil_moisture() {
  pinMode(SOIL_MOISTURE_PIN, INPUT);  // Пин для датчика влажности почвы в режиме входа

  showLoadingprogressBar("Soil moisture", true);
}
