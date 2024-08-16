void loop() {
  static TimerMs tmr(1000, true);  // задержка в 1000мс используемая вместо delay
  if (tmr.tick()) {                // запуск задержки

    light = analogRead(4);  // свет

    microclimate.humidity = dht.readHumidity();                                                                        // влажность
    microclimate.temperature = dht.readTemperature(isCorF);                                                            // температура
    microclimate.heatIndex = dht.convertFtoC(dht.computeHeatIndex(dht.readTemperature(true), microclimate.humidity));  // индекс тепла

    isDark = light > 300;                      // темно если меньше 300
    isCold = microclimate.temperature < 23.0;  // холодно если меньше 23
    isHot = microclimate.temperature > 24.0;   // жарко если больше 24

    //Действия в зависимости от значений//
    if (isDark) digitalWrite(lighting, HIGH);  // Если слишком темно, включаем светодиод на пине 13
    else digitalWrite(lighting, LOW);          // Иначе выключаем светодиод на пине 13
    if (isCold) digitalWrite(heating, HIGH);   // Если слишком холодно, включаем светодиод на пине 12
    else digitalWrite(heating, LOW);           // Иначе выключаем светодиод на пине 12
    if (isHot) digitalWrite(cooling, HIGH);    // Если слишком жарко, включаем светодиод на пине 11
    else digitalWrite(cooling, LOW);           // Иначе выключаем светодиод на пине 11
  }

  LEDdisplay();  // вывод на дисплей и сериал монитор
}