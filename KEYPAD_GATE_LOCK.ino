#include <Keypad.h>
#include <EEPROM.h>
#include "MyKeypadDisplay.h"

//Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 3;
 
//Array to represent keys on keypad
char hexKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
 
//Digital Pin Connections to Keypad
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3}; 

//Key Globals 
char    customKey;
bool    Unlocked=false;
String  defaultUnlockCode="0000";
String  unlockCode="0000";
int     unlockCodeLength=unlockCode.length();
const bool  DoPRINT=false;
const bool  DontShowDigits=true;
const char  NoDigit='.';

//Definitions
#define  AdminCode 1984 //SecretNumber Store in EEPROM at ADDRESS 0x05 Decimal use to test if unlock code has been set in EEPROM
#define  AdminIndex 5
#define  buzzerPIN 13    //Peizo Buzzer Pin for tone sounds
#define  RelayOUT A1  //Defines Pin A1 as the relay control

// Create Keypad object
Keypad customKeypad=Keypad(makeKeymap(hexKeys), rowPins, colPins, ROWS, COLS);

//Create Display Object
MyKeypadDisplay myDisplay;
 
//Initialises EEPROM 
void ZapEprom()
{
  EEPROM.begin();
  for(byte x=0;x<20;x++)
      EEPROM.put(x,0);
  EEPROM.end();
}

//OK Sound
void beep()
{
  tone(buzzerPIN, 2000,100); // Send 2KHz sound signal     // Stop sound
}

//NOT OK Sound
void boop()
{
  tone(buzzerPIN,400,1000); // Send 0.4KHz sound signal
}

bool IsKeyNumeric(char key)
{
  byte num = (byte)key;
  return (num>=48 && num <=57);
}

int GetEEPROMAdminCode()
{
  int AdminSecretPin;
  EEPROM.begin();
  AdminSecretPin=EEPROM.get(AdminIndex,AdminSecretPin);
  EEPROM.end();
  return AdminSecretPin;
}

int SetEEPROMAdminCode()
{
   int AdminSecretPin;
   EEPROM.begin();
   AdminSecretPin=EEPROM.put(AdminIndex, AdminCode);
   EEPROM.end();
   return AdminSecretPin;
}

//Useful for diagnosing further work if needed
void DumpEEPROM()
{    
  if(!DoPRINT) return;
  Serial.println("DUMP EEPROM");
  Serial.println("===========");
  Serial.println("Open Code");
  char c;
  String theOpenCode="";
  int digit;
  for(int x=0;x<unlockCodeLength;x++)
  {
    Serial.print((char)EEPROM.get(x,c)); 
    theOpenCode+=(char)EEPROM.get(x,c);
  }
  Serial.print("(");
  Serial.print(theOpenCode);
  Serial.println(")");

  Serial.println("AdminSecretPin CODE ");
  Serial.println(EEPROM.get(AdminIndex,digit),DEC);
  Serial.println();

  Serial.print("defaultUnlockCode : ");Serial.println(defaultUnlockCode);
  Serial.print("unlockCode : ");Serial.println(unlockCode);
  Serial.print("unlockCodeLength : ");Serial.println(unlockCodeLength);
}

//Used to retreive/match User Pin to open lock
String GetPin( uint8_t pinLength = 4, String pinToMatch = "", char* instruct16="", String failureDisplay="", char  token='*')
{
  char  customKey=char(0);
  String keysPressed="";
  uint8_t x=1;
  int timeoutMs=10000;
  long startMillis=millis();

  myDisplay.Clear();
  myDisplay.Print(instruct16,0,0);
  myDisplay.PrintChr(token,0,1);
  
  while( keysPressed.length()<=pinLength )
  {  
    customKey=customKeypad.getKey();
    if( millis()-startMillis>timeoutMs) return "TIMEOUT";
  
    if(customKey)
    {
      beep();
      startMillis=millis();
      if(DontShowDigits)
         myDisplay.PrintChr(NoDigit,x++,1);
      else
         myDisplay.PrintChr(customKey,x++,1);

      keysPressed+=String(customKey); 
    
      if(keysPressed.length()==pinLength)
      {       
        //If a Collected Pin is specified length and no match requirement then return String containing 
        if( keysPressed.length()==pinLength && pinToMatch.length()==0 ) return keysPressed;

        //If a Collected Pin is specified length and keysPressed Match  pinToMatch then return String "MATCH" else "FAIL" returned 
        if( keysPressed.length()==pinLength )
            if( keysPressed==pinToMatch) return "MATCH"; else return "FAIL";
      }
    }
  }
  x++;
}

String EnterSecetSumber()
{
   //Secret Number Is AdminCode
   //user enters the SecretNumber
  
   char keyPressed;
   String secret=String(AdminCode);
   String reply=GetPin(4, secret, "Enter Admin Code", "", '#');
   return reply;
}


bool SetNewAccessCode(char customKey )
{
   String reply=GetPin( 4, "", "New User Pin?", "", '#');
   if( reply=="FAIL" || reply=="TIMEOUT")
   {
     myDisplay.Clear();
     myDisplay.PrintStr(reply, 0,0);
     boop();
     delay(800);
     return false;
   }
   else
   {
   
      //Save new code returned
      EEPROM.begin();
      for( int x=0;x<unlockCodeLength;x++)
          EEPROM.put(x,reply.charAt(x));
      EEPROM.end();
      unlockCode=reply;
      myDisplay.Clear();
      myDisplay.Print("New Pin Saved", 0, 0);
      delay(2000);
      DumpEEPROM();
      return true;
   }
}

bool TryUnlockSequence( char customKey )
{
   String reply=GetPin(4,unlockCode,"Enter Access Pin","",'*');

   if( reply=="MATCH") 
   {
      long timeStartedMs=millis();
      int countdown = 8;

      //Annouce Door Unlocked
      delay(500);
      myDisplay.Clear();
      myDisplay.PrintStr("*Door Unlocked*", 0, 0); 
      digitalWrite(RelayOUT, HIGH); //Energise Relay
      //Annouce Countdown To Re-Lock 
      myDisplay.Print("Locking in", 0,1);
      for (int i=countdown; i >0 ; i--) 
      {
        myDisplay.PrintChr(char(48+i), 11,1);
        delay(1000);
        beep();
      }     
      digitalWrite(RelayOUT, LOW);//De-Energise Relay
      myDisplay.Clear();
      myDisplay.PrintStr("*Door LOCKED*", 0, 0); 
      delay(2000);
      return true;
   }

   if( reply=="TIMEOUT") 
   {
     boop();
     myDisplay.Clear();
     myDisplay.Print("TIMEOUT", 0, 0);
     delay(1000);
     return false;
   }
   else
   {
    delay(1500);
    myDisplay.Clear();
    myDisplay.Print("Wrong Pin", 0, 0);
    boop();
    delay(1000);
    return false;
   }

}

void setup() 
{
  //Testing or Development purposes only.
  //ZapEprom();

  pinMode(RelayOUT, OUTPUT);
  digitalWrite(RelayOUT, LOW);// TURN OFF SOLENOID SIGNAL

  //Setup Buzzer
  pinMode(buzzerPIN,OUTPUT); 

  // Setup serial monitor
  Serial.begin(9600);
  while(!Serial==1){}
  myDisplay.Init();
  delay(500);

  if(DoPRINT) 
  {
    DumpEEPROM();
  }

  //If AdminSecretPin code Does exist in the EEPROM then set unlockCode = EEPROM Stored unlockCode
  if(GetEEPROMAdminCode()==AdminCode)
  { 
    if(DoPRINT) 
    {
      Serial.print("Setup unlockCode=");Serial.println(unlockCode);
      Serial.print("Has AdminCode = ");Serial.println(GetEEPROMAdminCode());
    }

    unlockCode="";
    for(int x=0;x<unlockCodeLength;x++)
    {
      unlockCode+=(char)EEPROM.read(x);      
    }
  }
  else
  {
    // Write Open Code/AdminSecretPin Code to EEPROM (acknowledges the unlock code in the EEPROM is now valid.)
    SetEEPROMAdminCode();
    EEPROM.begin();
    for(int x=0;x<unlockCodeLength;x++)
    {
      EEPROM.put(x,(char) defaultUnlockCode.charAt(x) );      
    }
    unlockCode=defaultUnlockCode;
    EEPROM.end();
  }

  if( DoPRINT ) DumpEEPROM();
   myDisplay.DisplayDefaultEntry();

}
 

void loop() 
{
  
  customKey = customKeypad.getKey();

  //If Unlock *
  if(customKey==42 )
  {
    beep();
    myDisplay.BackLight(true);
    TryUnlockSequence(customKey);
    customKey=0;
    myDisplay.DisplayDefaultEntry();
  }   

  //If SetNewUnlock #   First Enter Sectret Number 'NNNN' and then Enter The Actual new number to be used.
  if(customKey==35 ) 
  {
     myDisplay.BackLight(true);
     beep();
      
    if( EnterSecetSumber()=="MATCH" )  
    {
      SetNewAccessCode(customKey);
    }
    else
    {
      boop();
    }
     customKey=0;   
     myDisplay.DisplayDefaultEntry();  
   }
}

