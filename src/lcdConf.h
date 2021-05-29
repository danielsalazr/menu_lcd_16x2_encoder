#include <Wire.h> // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

byte maxFila = 2;

byte CaracterCursor[8] = {
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00010,
  0b00100,
  0b01000,
  0b00000
};

void inicializarLcd(){
    lcd.begin (16, 2);
    lcd.init();  // initializing the LCD
    lcd.backlight(); // Enable or Turn On the backlight
    lcd.home();

    lcd.createChar(0, CaracterCursor);//crear el caracter cursor 
}
