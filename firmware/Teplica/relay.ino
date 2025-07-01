void setupRelay() {
  pcf.begin();
  for (uint8_t i = 0; i < 8; i++) {
    pcf.pinMode(i, OUTPUT);
    pcf.digitalWrite(i, LOW);
  }
}

// === 💡 Универсальная функция управления реле ===
void setRelay(uint8_t index, bool state) {
  if (relayState[index] != state) {
    relayState[index] = state;
    pcf.digitalWrite(index, state ? HIGH : LOW);
  }
}

// === 🌱 Главная логика климат-контроля ===
void updateRelay() {
  uint8_t hour = 0;  // текущее время (только часы)
  struct tm timeinfo = { 0 };
  timeinfo.tm_hour = hour;

  // --- 🕘 Выбор дневного или ночного режима ---
  bool isDay = hour >= 8 && hour < 20;
  float T_air_min = isDay ? T_air_day_min : T_air_night_min;
  float T_air_max = isDay ? T_air_day_max : T_air_night_max;

  // --- 🔥 Обогреватель ---
  // Включаем если:
  //  - температура воздуха ниже дневного/ночного минимума
  //  - температура почвы ниже минимального порога и воздух не перегрет
  // Выключаем, если обе температуры в норме
  bool heater_on = (data.airTemp < T_air_min) || (data.soilTemp < T_soil_min && data.airTemp < T_air_max);
  bool heater_off = (data.airTemp >= T_air_min && data.soilTemp >= T_soil_off);
  setRelay(RELAY_HEATER, heater_on && !heater_off);

  // --- 💨 Вентилятор (охлаждение) ---
  // Включаем при перегреве воздуха, отключаем при спаде температуры
  bool fan_on = data.airTemp > T_air_max;
  bool fan_off = data.airTemp < T_air_cool_off;
  setRelay(RELAY_FAN, fan_on && !fan_off);

  // --- 🌫 Увлажнитель воздуха ---
  // Включаем при низкой влажности, выключаем при достижении среднего уровня
  bool humid_on = data.airHum < H_air_min;
  bool humid_off = data.airHum > H_air_ok;
  setRelay(RELAY_HUMIDIFIER, humid_on && !humid_off);

  // --- 💧 Полив ---
  // Включаем при пересушке почвы, выключаем при достижении максимума
  bool pump_on = data.soilMois < M_soil_min;
  bool pump_off = data.soilMois > M_soil_max;
  setRelay(RELAY_PUMP, pump_on && !pump_off);

  // --- 💡 Досветка ---
  // Включается при недостатке света в вечерние часы (например, с 17 до 20)
  bool light_time = hour >= light_start_hour && hour < light_end_hour;
  bool light_low = data.lightLevel < light_min;
  setRelay(RELAY_LIGHT, light_low && light_time);

  // --- 🌬 Вентиляция по CO₂ ---
  // Включается при низком уровне CO₂, выключается при нормализации
  bool vent_on = data.CO2ppm < CO2_min;
  bool vent_off = data.CO2ppm > CO2_ok;
  setRelay(RELAY_VENT_CO2, vent_on && !vent_off);
}
