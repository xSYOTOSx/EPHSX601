
int var =0;
int foo[10] = {1, 1, 2, 2, 2,1,1,2,1,1};
int size;
String str;


#include <Adafruit_AW9523.h>
Adafruit_AW9523 aw;
uint8_t Pin1 = 1;  // 0 thru 15
uint8_t Pin2 = 2;
uint8_t Pin3 = 3;


void setup() 
{
  Serial.begin(115200);
  while (!Serial) delay(10); //waits for serial port to open
  

  if (! aw.begin(0x58)) 
  {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }

  Serial.println("AW9523 found!");
  aw.pinMode(Pin1, OUTPUT);
  aw.pinMode(Pin2, OUTPUT);
  aw.pinMode(Pin3, OUTPUT);
  //Serial.printf("Starting Value: %d \n", var);
  //size = sizeof(foo)/sizeof(int);
  //Serial.printf("there are %d vlaues in the foo array \n", size);
}

void loop() 
{

while (var < (sizeof(foo)/sizeof(int))) 
  {
    RGB_B(); delay(500);

    int rnd = random(2);
    bool test;
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
    delay(1000);

    RGB_reset(); delay(500);
    var++;
  }
}

void RGB_R()
{
aw.digitalWrite(Pin1, HIGH);
aw.digitalWrite(Pin2, LOW);
aw.digitalWrite(Pin3, LOW);
}
void RGB_B()
{
aw.digitalWrite(Pin1, LOW);
aw.digitalWrite(Pin2, HIGH);
aw.digitalWrite(Pin3, LOW);
}
void RGB_G()
{
aw.digitalWrite(Pin1, LOW);
aw.digitalWrite(Pin2, LOW);
aw.digitalWrite(Pin3, HIGH);
}

void RGB_reset()
{
aw.digitalWrite(Pin1, LOW);
aw.digitalWrite(Pin2, LOW);
aw.digitalWrite(Pin3, LOW); 
}
