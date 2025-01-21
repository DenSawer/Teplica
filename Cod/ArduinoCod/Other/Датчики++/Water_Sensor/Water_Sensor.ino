/*
 * This Arduino Nano ESP32 code was developed by newbiely.com
 *
 * This Arduino Nano ESP32 code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/arduino-nano-esp32/arduino-nano-esp32-water-sensor
 */

#define POWER_PIN  27 // Arduino Nano ESP32 pin D4 connected to sensor's VCC pin
#define SIGNAL_PIN 17 // Arduino Nano ESP32 pin A0 connected to sensor's signal pin
#define SENSOR_MIN 0
#define SENSOR_MAX 1

int value = 0; // variable to store the sensor value
int level = 0; // variable to store the water level

void setup() {
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);   // Configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
}

void loop() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  level = map(value, SENSOR_MIN, SENSOR_MAX, 0, 4); // 4 levels
  Serial.print("Water level: ");
  Serial.println(level);

  delay(1000);
}
