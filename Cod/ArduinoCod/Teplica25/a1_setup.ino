void setup() {

  Serial.begin(115200);  // Инициализация серийного порта со скоростью 115200 бод

  //---------------------------------Инициализация пинов----------------------------------------
  pinMode(LDR_PIN, INPUT);            // Пин для фоторезистора в режиме входа
  pinMode(LAMP_PIN, OUTPUT);          // Пин для лампы в режиме выхода
  pinMode(HEAT_PIN, OUTPUT);          // Пин для красного светодиода в режиме выхода
  pinMode(COOL_PIN, OUTPUT);          // Пин для синего светодиода в режиме выхода
  pinMode(SOIL_MOISTURE_PIN, INPUT);  // Пин для датчика влажности почвы в режиме входа

  //--------------------------------Инициализация система---------------------------------------
  Serial.println("Начало инициализации системы...");
  setup_DP();        // Инициализация LCD и 7seg disp
  setup_RTC();       // Инициализация RTC
  setup_SD();        // Инициализация SD
  setup_DHT();       // Инициализация DHT22
  setup_Ethernet();  // Инициализация Ethernet
  openSQL();         // Обработка SQL

  // Сообщение о завершении инициализации системы
  Serial.println("Инициализация системы завершена.");
}

void showLoadingProgress(const char *str, bool yes_no = false) {
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.print(' ');
  if (yes_no) lcd.write(char(0));  // ✔
  else {
    lcd.write(char(0x78));  // ✖
    tone(BIPER_PIN, 1000, 500);
    tone(BIPER_PIN, 0, 200);
    tone(BIPER_PIN, 1000, 500);
    //pip();
  }
  device++;
  float progress = (float(device) / float(allDevice)) * 100.0;
  uint8_t filledBlocks = progress / 6.25;
  lcd.setCursor(0, 1);
  for (int i = 0; i < filledBlocks; i++) {
    lcd.print(char(255));  // Полный блок █
  }
  for (int i = filledBlocks; i < 16; i++) {
    lcd.print(' ');  // Пустое место ▒
  }
  delay(500);
}

/*void setup_DP() {
  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
  lcd.init();          // Инициализация LCD дисплея
  lcd.backlight();     // Включение подсветки LCD дисплея
  lcd.clear();
  byte customChar[] = { 0x00, 0x00, 0x01, 0x02, 0x14, 0x08, 0x00, 0x00 };
  lcd.createChar(0, customChar);
  Serial.println("LCD: инициализация завершена.");  // Сообщение об успешной инициализации
  lcd.print("Инициализация системы");
}*/

void setup_DP() {

  // Проверка связи с TM1637
  bool tm1637_working = false;
  for (int brightness = 0; brightness <= 7; brightness++) {
    disp.brightness(brightness);  // Изменение яркости
    disp.displayInt(1234);        // Вывод числа для проверки
    delay(200);
    disp.clear();
    delay(200);
    tm1637_working = true;  // Если ошибок нет, считаем устройство рабочим
  }

  if (!tm1637_working) {
    Serial.println("Ошибка: TM1637 не отвечает. Проверьте подключение.");
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.print("Error TM1637");
  }

  // Если TM1637 работает, выводим сообщение
  disp.brightness(7);
  Serial.println("7-сегментный дисплей: инициализация завершена.");

  // Проверка наличия и автоматическое определение LCD дисплея
  Wire.begin();
  if (!Wire.requestFrom(LCD_ADDRESS, 1)) {
    Serial.println("Ошибка: LCD дисплей не найден. Проверьте подключение.");
    delay(1000);
    return;
  }

  // Проверка размера дисплея
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(1000);

  // Пробуем вывести текст на строки 2 и 4, чтобы понять, 1602 или 2004
  lcd.setCursor(0, 1);
  lcd.print("Тест строки 2");
  delay(1000);
  lcd.setCursor(0, 3);  // Если это 1602, курсор на строке 3 не должен работать
  lcd.print("Тест строки 4");
  delay(1000);

  // Анализ вывода
  bool is2004 = Wire.requestFrom(LCD_ADDRESS, 1);  // Проверка отклика устройства
  lcd.clear();
  uint8_t razmer = 0;
  if (is2004) {
    lcd.setCursor(0, 0);
    razmer = 2004;
    lcd = LiquidCrystal_I2C(LCD_ADDRESS, 20, 4);  // Переинициализация
  } else {
    lcd.setCursor(0, 0);
    razmer = 1602;
    lcd = LiquidCrystal_I2C(LCD_ADDRESS, 16, 2);  // Переинициализация
  }

  byte customChar[] = { 0x00, 0x00, 0x01, 0x02, 0x14, 0x08, 0x00, 0x00 };
  lcd.createChar(0, customChar);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print(razmer);

  delay(1500);

  lcd.clear();
  disp.clear();
  showLoadingProgress("Display", true);
  Serial.println("LCD ");
  Serial.print(razmer);
  Serial.print(": Инициализация завершена.");
}

void setup_DHT() {
  dht.begin();                                       // Запуск работы с датчиком DHT
  microclimate.temperature = dht.readTemperature();  // Чтение температуры
  microclimate.humidity = dht.readHumidity();        // Чтение влажности

  if (isnan(microclimate.temperature) || isnan(microclimate.humidity)) {
    Serial.println("Ошибка: датчик DHT не подключен или неисправен!");  // Ошибка с DHT22
    showLoadingProgress("DHT", false);
    return;
  } else {
    Serial.println("Датчик DHT успешно инициализирован.");
    showLoadingProgress("DHT", true);
  }
}

void setup_RTC() {
  if (!rtc.begin()) {                     // Проверка успешной инициализации RTC
    Serial.println("RTC не обнаружен!");  // Сообщение об ошибке, если RTC не обнаружен
    showLoadingProgress("RTC", false);
    return;
  }
  // Установка времени, если RTC не настроен
  if (!rtc.isrunning()) {                                                 // Проверка, работает ли RTC
    Serial.println("RTC не настроен, установка времени по компьютеру.");  // Сообщение об установке времени
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));                       // Установка времени по дате и времени компиляции
    showLoadingProgress("RTC time", true);
  } else {
    Serial.println("RTC: инициализация завершена.");
    showLoadingProgress("RTC", true);
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
    showLoadingProgress("Ethernet", false);
  }

  Serial.print("IP-адрес: ");
  Serial.println(Ethernet.localIP());*/

  showLoadingProgress("Ethernet", true);
}

void pip() {
#define tonePin BIPER_PIN

  tone(tonePin, 1318, 150);
  delay(150);
  tone(tonePin, 1318, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1318, 300);
  delay(300);
  tone(tonePin, 1568, 600);
  delay(600);
  tone(tonePin, 784, 600);
  delay(600);
  tone(tonePin, 1046, 450);  // 2й такт
  delay(450);
  tone(tonePin, 784, 150);
  delay(450);
  tone(tonePin, 659, 300);
  delay(450);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 987, 150);
  delay(300);
  tone(tonePin, 932, 150);
  delay(150);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 784, 210);  // 3й такт
  delay(210);
  tone(tonePin, 1318, 210);
  delay(210);
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1750, 300);
  delay(300);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1568, 150);
  delay(300);
  tone(tonePin, 1318, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  tone(tonePin, 987, 450);
  delay(450);
  tone(tonePin, 1046, 450);  // 4й такт
  delay(450);
  tone(tonePin, 784, 150);
  delay(450);
  tone(tonePin, 659, 300);
  delay(450);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 987, 150);
  delay(300);
  tone(tonePin, 932, 150);
  delay(150);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 784, 210);  // 5й такт
  delay(210);
  tone(tonePin, 1318, 210);
  delay(210);
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1750, 300);
  delay(300);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1568, 150);
  delay(300);
  tone(tonePin, 1318, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  tone(tonePin, 987, 450);
  delay(600);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  delay(300);  // 7ой
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 2093, 300);
  delay(300);
  tone(tonePin, 2093, 150);
  delay(150);
  tone(tonePin, 2093, 600);
  delay(600);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз ПОВТОР
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(450);
  tone(tonePin, 1244, 300);
  delay(450);
  tone(tonePin, 1174, 450);
  delay(450);
  tone(tonePin, 1046, 600);
  delay(1200);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  delay(300);  // 7ой
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 2093, 300);
  delay(300);
  tone(tonePin, 2093, 150);
  delay(150);
  tone(tonePin, 2093, 600);
  delay(600);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз ПОВТОР
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(450);
  tone(tonePin, 1244, 300);
  delay(450);
  tone(tonePin, 1174, 450);
  delay(450);
  tone(tonePin, 1046, 600);
  delay(1200);
  tone(tonePin, 1046, 150);  // 8ой такт
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 784, 600);
  delay(600);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(150);
  delay(1200);
  tone(tonePin, 1046, 150);  // 8ой такт
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 784, 600);
  delay(600);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(150);
  delay(1200);
  tone(tonePin, 1046, 450);  // 2й такт
  delay(450);
  tone(tonePin, 784, 150);
  delay(450);
  tone(tonePin, 659, 300);
  delay(450);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 987, 150);
  delay(300);
  tone(tonePin, 932, 150);
  delay(150);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 784, 210);  // 3й такт
  delay(210);
  tone(tonePin, 1318, 210);
  delay(210);
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1750, 300);
  delay(300);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1568, 150);
  delay(300);
  tone(tonePin, 1318, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  tone(tonePin, 987, 450);
  delay(450);
  tone(tonePin, 1046, 450);  // 4й такт
  delay(450);
  tone(tonePin, 784, 150);
  delay(450);
  tone(tonePin, 659, 300);
  delay(450);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 987, 150);
  delay(300);
  tone(tonePin, 932, 150);
  delay(150);
  tone(tonePin, 880, 300);
  delay(300);
  tone(tonePin, 784, 210);  // 5й такт
  delay(210);
  tone(tonePin, 1318, 210);
  delay(210);
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1750, 300);
  delay(300);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1568, 150);
  delay(300);
  tone(tonePin, 1318, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  tone(tonePin, 987, 450);
  delay(600);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  delay(300);  // 7ой
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 2093, 300);
  delay(300);
  tone(tonePin, 2093, 150);
  delay(150);
  tone(tonePin, 2093, 600);
  delay(600);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз ПОВТОР
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(450);
  tone(tonePin, 1244, 300);
  delay(450);
  tone(tonePin, 1174, 450);
  delay(450);
  tone(tonePin, 1046, 600);
  delay(1200);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(150);
  delay(300);  // 7ой
  tone(tonePin, 1568, 150);
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 2093, 300);
  delay(300);
  tone(tonePin, 2093, 150);
  delay(150);
  tone(tonePin, 2093, 600);
  delay(600);
  tone(tonePin, 1568, 150);  // 6ой Спуск вниз ПОВТОР
  delay(150);
  tone(tonePin, 1480, 150);
  delay(150);
  tone(tonePin, 1396, 150);
  delay(150);
  tone(tonePin, 1244, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(300);
  tone(tonePin, 830, 150);
  delay(150);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 150);
  delay(450);
  tone(tonePin, 1244, 300);
  delay(450);
  tone(tonePin, 1174, 450);
  delay(450);
  tone(tonePin, 1046, 600);
  delay(1200);
  tone(tonePin, 1046, 150);  // 8ой такт
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 784, 600);
  delay(600);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(150);
  delay(1200);
  tone(tonePin, 1046, 150);  // 8ой такт
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1318, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 880, 150);
  delay(150);
  tone(tonePin, 784, 600);
  delay(600);
  tone(tonePin, 1046, 150);
  delay(150);
  tone(tonePin, 1046, 300);
  delay(300);
  tone(tonePin, 1046, 150);
  delay(300);
  tone(tonePin, 784, 150);
  delay(150);
  tone(tonePin, 1174, 300);
  delay(300);
  tone(tonePin, 1046, 2400);
  delay(24000);
  delay(1200);
}