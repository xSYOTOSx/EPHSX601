
int var =0;
int sequence[10] = {1, 1, 2, 2, 2,1,1,2,1,1};
int size;
String str;
bool test;

#include <Adafruit_AW9523.h>

Adafruit_AW9523 aw1;
//Box # Color ie B1PR is Box 1 Pin for Red is 1
uint8_t B1PR = 1;  uint8_t B1PB = 2;  uint8_t B1PG = 3;  // Board 1
uint8_t B2PR = 4;  uint8_t B2PB = 5;  uint8_t B2PG = 6;  // Board 2
uint8_t B3PR = 7;  uint8_t B3PB = 8;  uint8_t B3PG = 9;  // Board 3
uint8_t B4PR = 10; uint8_t B4PB = 11; uint8_t B4PG = 12; // Board 4
uint8_t B5PR = 13; uint8_t B5PB = 14; uint8_t B5PG = 15; // Board 5

Adafruit_AW9523 aw2;
uint8_t B6PR = 1;   uint8_t B6PB = 2;   uint8_t B6PG = 3;   // Board 6
uint8_t B7PR = 4;   uint8_t B7PB = 5;   uint8_t B7PG = 6;   // Board 7
uint8_t B8PR = 7;   uint8_t B8PB = 8;   uint8_t B8PG = 9;   // Board 8
uint8_t B9PR = 10;  uint8_t B9PB = 11;  uint8_t B9PG = 12;  // Board 9
uint8_t B10PR = 13; uint8_t B10PB = 14; uint8_t B10PG = 15; // Board 10

Adafruit_AW9523 aw3;
uint8_t B11PR = 1;  uint8_t B11PB = 2;  uint8_t B11PG = 3;  // Board 11
uint8_t B12PR = 4;  uint8_t B12PB = 5;  uint8_t B12PG = 6;  // Board 12



void setup() 
{
  Serial.begin(115200);
  while (!Serial) delay(10); //waits for serial port to open
  

  if (! aw1.begin(0x58)) 
  {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }

  Serial.println("AW9523 found!");
  aw1.pinMode(B1PR, OUTPUT); aw1.pinMode(B1PB, OUTPUT); aw1.pinMode(B1PG, OUTPUT); // Board 1
  aw1.pinMode(B2PR, OUTPUT); aw1.pinMode(B2PB, OUTPUT); aw1.pinMode(B2PG, OUTPUT); // Board 2
  //Serial.printf("Starting Value: %d \n", var);
  //size = sizeof(foo)/sizeof(int);
  //Serial.printf("there are %d vlaues in the foo array \n", size);
}

void loop() 
{
  


  

while (var < (sizeof(sequence)/sizeof(int))) 
  {


    Serial.printf("sequence # %d wants box #%d moved \n", var,sequence[var]);
    // Tell what board we want to be interacted with
    
    MoveMe();
    Imoved();
    

    
  /*
    if (foo[var]=rnd) 
    {
      str ="YES";
      RGB_reset();
      RGB_G();
    }
    else
    {
      str="NO";
      RGB_reset();
      RGB_R();
    }
    
    Serial.printf("array foo[%d] = %d. Rand = %d. Match? %s \n",var,foo[var],rnd,str);
    //delay(1000);
    */
    delay(1000);
    RGB_reset(); delay(500);
    var++;
  }
  
}

void MoveMe()
{
  if(sequence[var]==1)  {setBoardColor(1,  LOW, HIGH, LOW); Serial.println("Setting Board 1 to blue");}
  if(sequence[var]==2)  {setBoardColor(2,  LOW, HIGH, LOW); Serial.println("Setting Board 2 to blue");}
  if(sequence[var]==3)  {setBoardColor(3,  LOW, HIGH, LOW); Serial.println("Setting Board 3 to blue");}
  if(sequence[var]==4)  {setBoardColor(4,  LOW, HIGH, LOW); Serial.println("Setting Board 4 to blue");}
  if(sequence[var]==5)  {setBoardColor(5,  LOW, HIGH, LOW); Serial.println("Setting Board 5 to blue");}
  if(sequence[var]==6)  {setBoardColor(6,  LOW, HIGH, LOW); Serial.println("Setting Board 6 to blue");}
  if(sequence[var]==7)  {setBoardColor(7,  LOW, HIGH, LOW); Serial.println("Setting Board 7 to blue");}
  if(sequence[var]==8)  {setBoardColor(8,  LOW, HIGH, LOW); Serial.println("Setting Board 8 to blue");}
  if(sequence[var]==9)  {setBoardColor(9,  LOW, HIGH, LOW); Serial.println("Setting Board 9 to blue");}
  if(sequence[var]==10) {setBoardColor(10, LOW, HIGH, LOW); Serial.println("Setting Board 0 to blue");}
  if(sequence[var]==11) {setBoardColor(11, LOW, HIGH, LOW); Serial.println("Setting Board 1 to blue");}
  if(sequence[var]==12) {setBoardColor(12, LOW, HIGH, LOW); Serial.println("Setting Board 2 to blue");}
}
void setBoardColor(int board, int red, int blue, int green) 
{
  if (board == 1)  {aw1.digitalWrite(B1PR, red);  aw1.digitalWrite(B1PB, blue);  aw1.digitalWrite(B1PG, green);} 
  if (board == 2)  {aw1.digitalWrite(B2PR, red);  aw1.digitalWrite(B2PB, blue);  aw1.digitalWrite(B2PG, green);} 
  if (board == 3)  {aw1.digitalWrite(B3PR, red);  aw1.digitalWrite(B3PB, blue);  aw1.digitalWrite(B3PG, green);} 
  if (board == 4)  {aw1.digitalWrite(B4PR, red);  aw1.digitalWrite(B4PB, blue);  aw1.digitalWrite(B4PG, green);} 
  if (board == 5)  {aw1.digitalWrite(B5PR, red);  aw1.digitalWrite(B5PB, blue);  aw1.digitalWrite(B5PG, green);} 
  if (board == 6)  {aw2.digitalWrite(B6PR, red);  aw2.digitalWrite(B6PB, blue);  aw2.digitalWrite(B6PG, green);} 
  if (board == 7)  {aw2.digitalWrite(B7PR, red);  aw2.digitalWrite(B7PB, blue);  aw2.digitalWrite(B7PG, green);} 
  if (board == 8)  {aw2.digitalWrite(B8PR, red);  aw2.digitalWrite(B8PB, blue);  aw2.digitalWrite(B8PG, green);} 
  if (board == 9)  {aw2.digitalWrite(B9PR, red);  aw2.digitalWrite(B9PB, blue);  aw2.digitalWrite(B9PG, green);} 
  if (board == 10) {aw2.digitalWrite(B10PR, red); aw2.digitalWrite(B10PB, blue); aw2.digitalWrite(B10PG, green);}
  if (board == 11) {aw3.digitalWrite(B11PR, red); aw3.digitalWrite(B11PB, blue); aw3.digitalWrite(B11PG, green);}
  if (board == 12) {aw3.digitalWrite(B12PR, red); aw3.digitalWrite(B12PB, blue); aw3.digitalWrite(B12PG, green);}
  }
void RGB_reset()
{
  aw1.digitalWrite(B1PR,  LOW); aw1.digitalWrite(B1PB,  LOW); aw1.digitalWrite(B1PG,  LOW);
  aw1.digitalWrite(B2PR,  LOW); aw1.digitalWrite(B2PB,  LOW); aw1.digitalWrite(B2PG,  LOW);
  aw1.digitalWrite(B3PR,  LOW); aw1.digitalWrite(B3PB,  LOW); aw1.digitalWrite(B3PG,  LOW);
  aw1.digitalWrite(B4PR,  LOW); aw1.digitalWrite(B4PB,  LOW); aw1.digitalWrite(B4PG,  LOW);
  aw1.digitalWrite(B5PR,  LOW); aw1.digitalWrite(B5PB,  LOW); aw1.digitalWrite(B5PG,  LOW);
  aw2.digitalWrite(B6PR,  LOW); aw1.digitalWrite(B6PB,  LOW); aw1.digitalWrite(B6PG,  LOW);
  aw2.digitalWrite(B7PR,  LOW); aw1.digitalWrite(B7PB,  LOW); aw1.digitalWrite(B7PG,  LOW);
  aw2.digitalWrite(B8PR,  LOW); aw1.digitalWrite(B8PB,  LOW); aw1.digitalWrite(B8PG,  LOW);
  aw2.digitalWrite(B9PR,  LOW); aw1.digitalWrite(B9PB,  LOW); aw1.digitalWrite(B9PG,  LOW);
  aw2.digitalWrite(B10PR, LOW); aw1.digitalWrite(B10PB, LOW); aw1.digitalWrite(B10PG, LOW);
  aw3.digitalWrite(B11PR, LOW); aw1.digitalWrite(B11PB, LOW); aw1.digitalWrite(B11PG, LOW);
  aw3.digitalWrite(B12PR, LOW); aw1.digitalWrite(B12PB, LOW); aw1.digitalWrite(B12PG, LOW);
}
int Imoved()
{
int rnd = random(2); Serial.printf("I moved Box number %d \n ");
return rnd;
}
void MovedCorrect(int rnd,int var, int sequence)
{
Serial.printf("Desired: ",sequence[var]);
}
