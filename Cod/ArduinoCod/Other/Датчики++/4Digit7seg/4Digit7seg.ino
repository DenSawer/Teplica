#include <TM1637Display.h>

// Укажите пины для подключения CLK и DIO
#define CLK 21
#define DIO 22

TM1637Display display(CLK, DIO);

void setup() {
  display.setBrightness(0x0f);  // Установите яркость (максимум)
  display.showNumberDec(1234);  // Показать число 1234
}

void loop() {
  // Можно добавить код для обновления дисплея или выполнения других задач
}
