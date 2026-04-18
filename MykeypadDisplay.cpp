#include <Wire.h>
#include "MyKeypadDisplay.h"

 LiquidCrystal_I2C lcd(0x27,16,2);
   //AlternateAddress (0x3F,16,2);
//Constructor
MyKeypadDisplay::MyKeypadDisplay()
{
  //Any additional initialiser code.
};

void MyKeypadDisplay::Init()
{
  lcd.init();
  lcd.backlight();
  delay(500);
};

//Experimental Not currently used to sense the address of the Crystal Display
bool MyKeypadDisplay::i2CAddrTest(uint8_t addr) 
{
 Wire.begin();
 Wire.beginTransmission(addr);
 if (Wire.endTransmission() == 0) 
 {
   return true;
 }
 return false;
};


void MyKeypadDisplay::DisplayDefaultEntry()
{
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.printstr("Open * / Admin #");
}

//Not Currently called - Channel Plug
void MyKeypadDisplay::Splash()
{
  lcd.setCursor(0,0);
  lcd.printstr("TELsBench");
  lcd.setCursor(0,1);
  lcd.printstr("Keypad/Lock");
};

void MyKeypadDisplay::Clear()
{
   lcd.clear(); 
};

void MyKeypadDisplay::Print( char* message, uint8_t x, uint8_t y)
{
   lcd.setCursor(x,y); 
   lcd.printstr(message);
};

void MyKeypadDisplay::PrintStr( String message, uint8_t x, uint8_t y)
{
   lcd.setCursor(x,y); 
   lcd.print(message);
};

void MyKeypadDisplay::PrintChr( char c , uint8_t x, uint8_t y)
{
   lcd.setCursor(x,y); 
   lcd.print(c);
};

void MyKeypadDisplay::SetCursor( uint8_t x, uint8_t y)
{
   lcd.setCursor(x,y); 
};

 void MyKeypadDisplay::BackLight(bool On = true)
 {
    if( On)
       lcd.backlight();
    else
       lcd.noBacklight();
 }



