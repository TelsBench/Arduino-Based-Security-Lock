#include <Arduino.h>
#include <LiquidCrystal_I2C.h>



class MyKeypadDisplay
{
  private:
  
  

  public:
  

  bool i2CAddrTest(uint8_t addr);
  

  MyKeypadDisplay();
  void Init();
  void Splash();
  void Clear();
  void Print( char* message, uint8_t x, uint8_t y);
  void MyKeypadDisplay::PrintChr( char c , uint8_t x, uint8_t y);
  void MyKeypadDisplay::SetCursor( uint8_t x, uint8_t y);
  void MyKeypadDisplay::DisplayDefaultEntry();
  void MyKeypadDisplay::PrintStr( String message, uint8_t x, uint8_t y);
  void MyKeypadDisplay::BackLight(bool On = true);


};



