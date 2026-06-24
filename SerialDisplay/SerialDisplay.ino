#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  // Initialize the LCD
  lcd.init();

  // Turn on the backlight
  //lcd.backlight();

  // First line
  lcd.setCursor(0, 0);
  lcd.print("Hello, World!");

  // Second line
  lcd.setCursor(0, 1);
  lcd.print("Arduino UNO");
}

void loop()
{
  // Nothing here
}
