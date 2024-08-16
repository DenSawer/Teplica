void setup()
{
  Serial.begin(9600);
  dht.begin();

  lcd.init(); // инициализируем экран
  lcd.backlight(); // включаем подсветку

  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)

  Serial.println("LABEL, Time, Minute, Temp, tooCold, tooHot, Light, tooDark, vlaga, index tepla");  // Вывод заголовка столбцов на //последовательный порт
  pinMode(lighting, OUTPUT); // Настраиваем пин 13 как выход
  pinMode(heating, OUTPUT); // Настраиваем пин 12 как выход
  pinMode(cooling, OUTPUT); // Настраиваем пин 11 как выход
}