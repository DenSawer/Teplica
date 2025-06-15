#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

char espID[] = "123456";  // Код теплицы

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Сканирование сетей Wi-Fi...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("Сети не найдены");
    return;
  }

  // Показать найденные сети
  for (int i = 0; i < n; ++i) {
    Serial.printf("[%d] SSID: %s | RSSI: %d | %s\n", i + 1,
                  WiFi.SSID(i).c_str(),
                  WiFi.RSSI(i),
                  (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Открытая" : "Защищенная");
  }

  // Подключиться к первой открытой сети
  for (int i = 0; i < n; ++i) {
    if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
      String ssid = WiFi.SSID(i);
      Serial.println("\nПопытка подключения к открытой сети: " + ssid);
      WiFi.begin(ssid.c_str());

      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(1000);
        Serial.print(".");
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nУспешно подключено к: " + ssid);
        Serial.println("IP адрес: " + WiFi.localIP().toString());

        // Отправка JSON
        sendJSON();
        return;  // После отправки JSON — завершение setup()
      } else {
        Serial.println("\nНе удалось подключиться к сети: " + ssid);
      }
    }
  }

  Serial.println("Нет открытых сетей для подключения.");
}

void loop() {
  // Ничего
}

void sendJSON() {
  WiFiClientSecure client;
  client.setInsecure();  // Отключаем проверку SSL

  HTTPClient https;

  String url = "https://teplica.denserver.ru/api/upload/" + String(espID);
  Serial.println("Отправка JSON на: " + url);

  // Здесь формируем JSON строку (можно заменить на реальную телеметрию)
  String jsonData = "{\"date\":\"11/04/2025\",\"time\":\"14:00:00\",\"airTemp\":19,\"soilTemp\":19.6875,\"airHum\":45,\"soilMois\":52,\"lightLevel\":1031,\"CO2ppm\":200}";

  if (https.begin(client, url)) {
    https.addHeader("Content-Type", "application/json");

    int httpCode = https.POST(jsonData);

    if (httpCode > 0) {
      Serial.printf("Код ответа сервера: %d\n", httpCode);
      String response = https.getString();
      Serial.println("Ответ сервера: " + response);
    } else {
      Serial.printf("Ошибка POST: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.println("Ошибка инициализации HTTPS-соединения");
  }
}
