
// ESP Board MAC Address:  C8:C9:A3:5B:DA:BB
#include <ESP8266WiFi.h> 
#include <espnow.h>
#include <Adafruit_AW9523.h> // for multiplexer

//Box # Color ie B1PR is Box 1 Pin for Red is 1
Adafruit_AW9523 aw1;
uint8_t B1PR = 0;  uint8_t B1PG = 1;  uint8_t B1PB = 2;   // Board 1
uint8_t B2PR = 3;  uint8_t B2PG = 4;  uint8_t B2PB = 5;   // Board 2
uint8_t B3PR = 6;  uint8_t B3PG = 7;  uint8_t B3PB = 8;   // Board 3
uint8_t B4PR = 9;  uint8_t B4PG = 10; uint8_t B4PB = 11;  // Board 4
Adafruit_AW9523 aw2;
uint8_t B5PR = 0;   uint8_t B5PG = 1;   uint8_t B5PB = 2;   // Board 5
uint8_t B6PR = 3;   uint8_t B6PG = 4;   uint8_t B6PB = 5;   // Board 6
uint8_t B7PR = 6;   uint8_t B7PG = 7;   uint8_t B7PB = 8;   // Board 7
uint8_t B8PR = 14;  uint8_t B8PG = 15;  uint8_t B8PB = 13;  // Board 8
Adafruit_AW9523 aw3;
uint8_t B9PR =  0;  uint8_t B9PG =   1; uint8_t B9PB =   9; // Board 9
uint8_t B10PR = 3;  uint8_t B10PG =  4; uint8_t B10PB =  5;// Board 10
uint8_t B11PR = 6;  uint8_t B11PG =  7; uint8_t B11PB =  8; // Board 11
uint8_t B12PR = 13; uint8_t B12PG = 15; uint8_t B12PB = 14; // Board 12





void setup() 
{
  Serial.begin(115200); while(! Serial) delay(1);
  Serial.println("Hello World!");
  
  //Initalizes the multiplexer I2C
  if (! aw1.begin(0x58)) { Serial.println("0x58 not found?");while (1) delay(10);} Serial.println("0x58 found!");//0x58 is default adress
  if (! aw2.begin(0x59)) { Serial.println("0x59 not found?");while (1) delay(10);} Serial.println("0x59 found!");//0x59 (A0 shorted)
  if (! aw3.begin(0x5A)) { Serial.println("0x5A not found?");while (1) delay(10);} Serial.println("0x5A found!");//0x5A (A1 shorted)

  //Defines pins on the multiplexer
  for(int i=0; i<=15; i++)
  {
    aw1.pinMode(i, OUTPUT);
    aw2.pinMode(i, OUTPUT);
    aw3.pinMode(i, OUTPUT);
  }

  RGB_test();
  


}

void loop()
{




  
}



void setBoardColor(int board, int red, int green, int blue) //RGB
{
  if (board == 1)  {aw1.digitalWrite(B1PR, red);  aw1.digitalWrite(B1PG, green);  aw1.digitalWrite(B1PB, blue);  } 
  if (board == 2)  {aw1.digitalWrite(B2PR, red);  aw1.digitalWrite(B2PG, green);  aw1.digitalWrite(B2PB, blue);  } 
  if (board == 3)  {aw1.digitalWrite(B3PR, red);  aw1.digitalWrite(B3PG, green);  aw1.digitalWrite(B3PB, blue);  } 
  if (board == 4)  {aw1.digitalWrite(B4PR, red);  aw1.digitalWrite(B4PG, green);  aw1.digitalWrite(B4PB, blue);  } 
  if (board == 5)  {aw2.digitalWrite(B5PR, red);  aw2.digitalWrite(B5PG, green);  aw2.digitalWrite(B5PB, blue);  } 
  if (board == 6)  {aw2.digitalWrite(B6PR, red);  aw2.digitalWrite(B6PG, green);  aw2.digitalWrite(B6PB, blue);  } 
  if (board == 7)  {aw2.digitalWrite(B7PR, red);  aw2.digitalWrite(B7PG, green);  aw2.digitalWrite(B7PB, blue);  } 
  if (board == 8)  {aw2.digitalWrite(B8PR, red);  aw2.digitalWrite(B8PG, green);  aw2.digitalWrite(B8PB, blue);  } 
  if (board == 9)  {aw3.digitalWrite(B9PR, red);  aw3.digitalWrite(B9PG, green);  aw3.digitalWrite(B9PB, blue);  } 
  if (board == 10) {aw3.digitalWrite(B10PR, red); aw3.digitalWrite(B10PG, green); aw3.digitalWrite(B10PB, blue); }
  if (board == 11) {aw3.digitalWrite(B11PR, red); aw3.digitalWrite(B11PG, green); aw3.digitalWrite(B11PB, blue); }
  if (board == 12) {aw3.digitalWrite(B12PR, red); aw3.digitalWrite(B12PG, green); aw3.digitalWrite(B12PB, blue); }
}

void RGB_test()
{
  RGB_reset();
  for(int i=5; i<=12; i++)
  {
    for(int j=1; j<=3; j++)
    {
    
    if(j=1){setBoardColor(i, HIGH, LOW, LOW); Serial.printf("board %d set to RED \n",i);  delay(500);RGB_reset();}
    if(j=2){setBoardColor(i, LOW, HIGH, LOW); Serial.printf("board %d set to Green \n",i);delay(500);RGB_reset();}
    if(j=3){setBoardColor(i, LOW, LOW, HIGH); Serial.printf("board %d set to BLUE \n",i); delay(500);RGB_reset();}
    }
  }
}

void RGB_reset()
{
  for(int i=0; i<=15; i++)
  {
    aw1.digitalWrite(i, LOW);
    aw2.digitalWrite(i, LOW);
    aw3.digitalWrite(i, LOW);
  }
}
