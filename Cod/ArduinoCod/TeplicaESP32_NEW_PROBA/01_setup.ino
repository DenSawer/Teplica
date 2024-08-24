void setup() {
  Serial.begin(115200);  // Инициализация серийного порта со скоростью 115200 бод

  Serial.println("Начало инициализации системы...");

  // Инициализация пинов
  pinMode(LDR_PIN, INPUT);            // Пин для фоторезистора в режиме входа
  pinMode(LAMP_PIN, OUTPUT);          // Пин для лампы в режиме выхода
  pinMode(HEAT_PIN, OUTPUT);          // Пин для красного светодиода в режиме выхода
  pinMode(COOL_PIN, OUTPUT);          // Пин для синего светодиода в режиме выхода
  pinMode(SOIL_MOISTURE_PIN, INPUT);  // Пин для датчика влажности почвы в режиме входа

  // Инициализация DHT
  dht.begin();                                      // Запуск работы с датчиком DHT
  Serial.println("DHT: инициализация завершена.");  // Сообщение об успешной инициализации

  // Инициализация LCD
  lcd.init();                                       // Инициализация LCD дисплея
  lcd.backlight();                                  // Включение подсветки LCD дисплея
  lcd.clear();
  Serial.println("LCD: инициализация завершена.");  // Сообщение об успешной инициализации

  // Инициализация RTC
  if (!rtc.begin()) {                     // Проверка успешной инициализации RTC
    Serial.println("RTC не обнаружен!");  // Сообщение об ошибке, если RTC не обнаружен
    while (1)
      ;  // Остановка выполнения программы
  }

  // Установка времени, если RTC не настроен
  if (!rtc.isrunning()) {                                                 // Проверка, работает ли RTC
    Serial.println("RTC не настроен, установка времени по компьютеру.");  // Сообщение об установке времени
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));                       // Установка времени по дате и времени компиляции
  }
  Serial.println("RTC: инициализация завершена.");  // Сообщение об успешной инициализации

  // Инициализация Ethernet
  Serial.println("Начало инициализации Ethernet...");
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
  }

  // Сообщение о завершении инициализации системы
  Serial.println("Инициализация системы завершена.");
}