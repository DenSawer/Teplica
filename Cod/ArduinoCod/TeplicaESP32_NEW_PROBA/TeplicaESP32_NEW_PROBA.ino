#include <Wire.h>                 // Библиотека для работы с шиной I2C
#include <LiquidCrystal_I2C.h>    // Библиотека для работы с LCD по I2C
#include <RTClib.h>               // Библиотека для работы с RTC (часы реального времени)
#include <Adafruit_Sensor.h>      // Библиотека для работы с датчиками Adafruit
#include <DHT.h>                  // Библиотека для работы с датчиком DHT
#include <DHT_U.h>                // Дополнительная библиотека для DHT
#include <SPI.h>                  // Библиотека для работы с SPI
#include <EthernetENC.h>          // Библиотека для работы с Ethernet модулем ENC28J60

// Определение пинов для подключения различных устройств
#define LDR_PIN 34          // Пин для подключения фоторезистора
#define LAMP_PIN 25         // Пин для управления лампой
#define DHT_PIN 4           // Пин для подключения датчика температуры и влажности DHT22
#define HEAT_LED_PIN 32     // Пин для управления подогревом (красный светодиод)
#define COOL_LED_PIN 33     // Пин для управления охлаждением (синий светодиод)
#define SOIL_MOISTURE_PIN 35 // Пин для подключения датчика влажности почвы HW-080

// Определение типа датчика DHT
#define DHTTYPE DHT22       // Указываем, что используем датчик DHT22

// Создание объекта для работы с датчиком DHT
DHT dht(DHT_PIN, DHTTYPE);

// Установка количества столбцов и строк для LCD дисплея
int lcdColumns = 16;        // 16 столбцов
int lcdRows = 2;            // 2 строки

// Создание объекта для работы с LCD дисплеем с адресом 0x27
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Создание объекта для работы с RTC (часы реального времени)
RTC_DS1307 rtc;

// Настройка Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC адрес устройства
IPAddress ip(192, 168, 1, 28);                        // Статический IP адрес
IPAddress dnsServer(192, 168, 1, 1);                  // IP адрес DNS сервера
IPAddress gw(192, 168, 1, 1);                         // IP адрес шлюза
IPAddress sn(255, 255, 255, 0);                       // Маска подсети
char server[] = "httpbin.org";                        // Сервер для подключения по HTTP
EthernetClient client;                                // Создание клиента для Ethernet

unsigned long beginMicros, endMicros;                 // Переменные для измерения скорости передачи данных
unsigned long byteCount = 0;                          // Счетчик принятых байт
bool printWebData = true;                             // Флаг для вывода данных в Serial Monitor

void setup() {
  Serial.begin(115200);          // Инициализация серийного порта со скоростью 115200 бод
  
  // Сообщение об инициализации системы
  Serial.println("Начало инициализации системы...");

  // Инициализация пинов
  pinMode(LDR_PIN, INPUT);       // Пин для фоторезистора в режиме входа
  pinMode(LAMP_PIN, OUTPUT);     // Пин для лампы в режиме выхода
  pinMode(HEAT_LED_PIN, OUTPUT); // Пин для красного светодиода в режиме выхода
  pinMode(COOL_LED_PIN, OUTPUT); // Пин для синего светодиода в режиме выхода
  pinMode(SOIL_MOISTURE_PIN, INPUT); // Пин для датчика влажности почвы в режиме входа

  // Инициализация DHT22
  dht.begin();                   // Запуск работы с датчиком DHT22
  Serial.println("DHT22: инициализация завершена."); // Сообщение об успешной инициализации

  // Инициализация LCD
  lcd.init();                    // Инициализация LCD дисплея
  lcd.backlight();               // Включение подсветки LCD дисплея
  Serial.println("LCD: инициализация завершена."); // Сообщение об успешной инициализации

  // Инициализация RTC
  if (!rtc.begin()) {            // Проверка успешной инициализации RTC
    Serial.println("RTC не обнаружен!"); // Сообщение об ошибке, если RTC не обнаружен
    while (1);                   // Остановка выполнения программы
  }

  // Установка времени, если RTC не настроен
  if (!rtc.isrunning()) {        // Проверка, работает ли RTC
    Serial.println("RTC не настроен, установка времени по компьютеру."); // Сообщение об установке времени
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Установка времени по дате и времени компиляции
  }
  Serial.println("RTC: инициализация завершена."); // Сообщение об успешной инициализации

  // Инициализация Ethernet
  Serial.println("Начало инициализации Ethernet...");
  Ethernet.init(5);  // Настройка для MKR ETH Shield (используется CS-пин 5)
  if (Ethernet.begin(mac) == 0) { // Если не удалось получить IP через DHCP
    Serial.println("Ethernet: не удалось получить IP через DHCP, используется статический IP.");
    Ethernet.begin(mac, ip, dnsServer, gw, sn); // Использование статического IP
    Serial.println("Ethernet: инициализация завершена.");
  } else {
    Serial.println("Ethernet: получен IP через DHCP.");
  }

  delay(5000); // Задержка для завершения инициализации

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
    client.println("GET /get HTTP/1.1");    // Отправка HTTP GET-запроса
    client.println("Host: httpbin.org");    // Указание хоста
    client.println("Connection: close");    // Закрытие соединения после завершения запроса
    client.println();                       // Пустая строка для завершения заголовков запроса
    beginMicros = micros();                 // Запоминание времени начала передачи данных
  } else {
    Serial.println("Ошибка подключения к серверу.");
  }

  // Сообщение о завершении инициализации системы
  Serial.println("Инициализация системы завершена.");
}

void loop() {
  // Чтение показаний с фоторезистора
  int ldrValue = analogRead(LDR_PIN);  // Чтение аналогового значения с пина фоторезистора

  // Включение или выключение лампы на основе показаний фоторезистора
  if (ldrValue > 1500) {
    digitalWrite(LAMP_PIN, HIGH);  // Включаем лампу
  } else {
    digitalWrite(LAMP_PIN, LOW);   // Выключаем лампу
  }

  // Чтение температуры и влажности с DHT22
  float temperature = dht.readTemperature();  // Чтение температуры
  float humidity = dht.readHumidity();        // Чтение влажности

  // Проверка, удалось ли прочитать данные с датчика DHT22
  if (!isnan(humidity) && !isnan(temperature)) {
    // Управление подогревом и охлаждением на основе температуры
    if (temperature < 28) {
      digitalWrite(HEAT_LED_PIN, HIGH);  // Включаем подогрев (красный светодиод)
      digitalWrite(COOL_LED_PIN, LOW);   // Выключаем охлаждение (синий светодиод)
    } else if (temperature > 30) {
      digitalWrite(HEAT_LED_PIN, LOW);   // Выключаем подогрев (красный светодиод)
      digitalWrite(COOL_LED_PIN, HIGH);  // Включаем охлаждение (синий светодиод)
    } else {
      digitalWrite(HEAT_LED_PIN, LOW);   // Выключаем подогрев
      digitalWrite(COOL_LED_PIN, LOW);   // Выключаем охлаждение
    }

    // Чтение данных с датчика влажности почвы
    int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN); // Чтение аналогового значения с пина датчика влажности почвы

    // Получение текущего времени с RTC
    DateTime now = rtc.now(); // Получение текущего времени от RTC

    // Вывод информации на дисплей
    lcd.clear();                     // Очистка дисплея
    lcd.setCursor(0, 0);            // Установка курсора в начало первой строки
    lcd.print("Temp: ");            // Вывод текста "Temp: "
    lcd.print(temperature);         // Вывод температуры
    lcd.print(" C");                // Вывод единицы измерения (градусы Цельсия)

    lcd.setCursor(0, 1);            // Установка курсора в начало второй строки
    lcd.print("Hum: ");             // Вывод текста "Hum: "
    lcd.print(humidity);            // Вывод влажности
    lcd.print("%");                 // Вывод единицы измерения (проценты)

    lcd.setCursor(9, 1);            // Установка курсора для вывода времени
    lcd.print(now.hour());          // Вывод часов
    lcd.print(':');                 // Вывод разделителя ":"
    lcd.print(now.minute());        // Вывод минут

    // Вывод информации в монитор порта
    Serial.print("LDR Value: ");
    Serial.println(ldrValue);       // Вывод значения фоторезистора
    Serial.print("Temperature: ");
    Serial.print(temperature);      // Вывод температуры
    Serial.println(" *C");
    Serial.print("Humidity: ");
    Serial.print(humidity);         // Вывод влажности
    Serial.println(" %");
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoistureValue); // Вывод влажности почвы
    Serial.println();
    Serial.print("Current Time: ");
    Serial.print(now.hour());       // Вывод текущего времени (часы)
    Serial.print(':');
    Serial.print(now.minute());     // Вывод текущего времени (минуты)
    Serial.print(':');
    Serial.print(now.second());     // Вывод текущего времени (секунды)
    Serial.println();
    Serial.println("--------------------");
  } else {
    Serial.println(F("Ошибка чтения данных с DHT22!")); // Вывод сообщения об ошибке, если не удалось прочитать данные
    lcd.clear();                // Очистка дисплея
    lcd.setCursor(0, 0);        // Установка курсора в начало первой строки
    lcd.print("DHT22 Error!");  // Вывод сообщения об ошибке на дисплей
    delay(2000);                // Задержка для отображения сообщения
  }

  // Обработка данных от сервера
  int len = client.available();  // Проверка доступности данных для чтения
  if (len > 0) {
    byte buffer[80];             // Буфер для хранения полученных данных
    if (len > 80) len = 80;      // Ограничение длины данных до 80 байт
    client.read(buffer, len);    // Чтение данных в буфер
    if (printWebData) {
      Serial.write(buffer, len); // Вывод данных в Serial Monitor
    }
    byteCount = byteCount + len; // Увеличение счетчика принятых байт
  }

  // Отключение при завершении связи с сервером
  if (!client.connected()) {
    endMicros = micros();        // Запоминание времени окончания передачи данных
    Serial.println();
    Serial.println("Отключение.");
    client.stop();               // Остановка клиента
    Serial.print("Получено ");
    Serial.print(byteCount);     // Вывод количества полученных байт
    Serial.print(" байт за ");
    float seconds = (float)(endMicros - beginMicros) / 1000000.0; // Расчет времени передачи данных
    Serial.print(seconds, 4);    // Вывод времени передачи данных
    float rate = (float)byteCount / seconds / 1000.0; // Расчет скорости передачи данных
    Serial.print(", скорость = ");
    Serial.print(rate);          // Вывод скорости передачи данных
    Serial.print(" кбайт/сек");
    Serial.println();
    
    // Попытка повторного подключения к серверу
    if (client.connect(server, 80)) {
      Serial.println("Повторное подключение!");
      client.println("GET /get HTTP/1.1");    // Отправка HTTP GET-запроса
      client.println("Host: httpbin.org");    // Указание хоста
      client.println("Connection: close");    // Закрытие соединения после завершения запроса
      client.println();                       // Пустая строка для завершения заголовков запроса
      beginMicros = micros();                 // Запоминание времени начала передачи данных
    } else {
      Serial.println("Ошибка повторного подключения.");
    }
  }

  // Задержка перед следующим обновлением
  delay(2000);  // Задержка на 2 секунды
}
