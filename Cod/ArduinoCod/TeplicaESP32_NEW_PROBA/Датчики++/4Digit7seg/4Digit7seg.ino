#include <GyverTM1637.h>

// Укажите пины для подключения CLK и DIO
#define CLK 21
#define DIO 22

GyverTM1637 disp(CLK, DIO);

void setup() {
  disp.brightness(7);
  disp.displayInt(8888);
  disp.clear();
}

void loop() {
  for (int i = 0; i <= 9; i++) {
    disp.displayByte(i,i,i,i);        // Вывод числа для проверки
    delay(200);
    disp.clear();
    delay(1000);
  }
}
