/*********************************************************************************************
  This code is heavilly dependent on Rui Santos code on randomnerdtutorials.com. For complete 
  project details visit https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/.
  
  From here orginial code: "Permission is hereby granted, free of charge, to any person 
  obtaining a copy of this software and associated documentation files. The above copyright
  notice and this permission notice shall be included in all copies or substantial portions 
  of the Software."

  Authers: Ryan Coppens & Thresa Kelly
*********************************************************************************************/
// ESP Board MAC Address:  C8:C9:A3:5B:DA:BB
// This is the liberry for the WIFI in the esp8266
#include <ESP8266WiFi.h>
#include <espnow.h>

// this the the LED multiplexer libery. I am setting up all the variable names 
// for the multiplexer
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

// Setting Up the OLED for testing Purposes
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Not sure howe were gonna impleament this but we shall see. But for now we have it defined here
int sequence[10] = {1, 1, 2, 2, 2,1,1,2,1,1};
const unsigned long timeout = 5000;
unsigned long previousMillis = 0;
int var =0;//global counter. May have a better way if implementing this

/*********************************************************************************************
This is the same as the senders. Were making the struct in the same was as the sender. THese 
two need to match. After that we name 16 of these structs. One for the message being passed to
us and than 15, one for each board. Then we put each of the structs for each sender into an 
array of structs. 
*********************************************************************************************/
typedef struct struct_message 
{
    int id;
    float x;
    float y;
    float z;
    float m;
    bool isMoving;
} struct_message;
// Create a struct_message called myData
struct_message myData;
// Create a structure to hold the readings from each board
struct_message board1; struct_message board2; struct_message board3;
struct_message board4; struct_message board5; struct_message board6;
struct_message board7; struct_message board8; struct_message board9;
struct_message board10; struct_message board11; struct_message board12;
struct_message board13; struct_message board14; struct_message board15;
// Create an array with all the structures
struct_message boardsStruct[15] = {board1, board2,board3,board4,board5, board6,board7,board8,
                                    board9, board10,board11,board12,board13, board14,board15};


/*********************************************************************************************
The function is called onDataRecv() and should accept several parameters as follows:
  - Get the sender board’s MAC address:
  - Copy the content of the incomingData data variable into the myData variable.
  - Each board is identified by its myData.id so we can call it in the boardsStruct array
  - myData.id-1 allows us to start in boardStruct[0] when filling in the array
  - For debugging purposes, is then printed in the Serial.
*********************************************************************************************/
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) 
{
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID: %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id].x = myData.x;
  boardsStruct[myData.id].y = myData.y;
  boardsStruct[myData.id].z = myData.z;
  boardsStruct[myData.id].m = myData.m;
  boardsStruct[myData.id].isMoving = myData.isMoving;
  Serial.print("x value: "); Serial.println(boardsStruct[myData.id].x);
  Serial.print("y value: "); Serial.println(boardsStruct[myData.id].y);
  Serial.print("z value: "); Serial.println(boardsStruct[myData.id].z);
  Serial.print("m value: "); Serial.println(boardsStruct[myData.id].m);
  Serial.print("T/F value: "); Serial.println(boardsStruct[myData.id].isMoving);
  Serial.println();
}

void setup() 
{
  Serial.begin(115200); while(! Serial);Serial.println("Hello World! Serial Has Begun");

  /*********************************************************************************************
  - Set the device as a Wi-Fi Station and disconnect Wi-Fi.
  - Initialize ESP-NOW:
  - This is a receiver board, so we’ll set it to ESP_NOW_ROLE_SLAVE
  - Register for a callback function OnDataRecv() that will be called when data is received.
  *********************************************************************************************/
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Device set as a Wi-Fi Station");
  if (esp_now_init() != 0) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Setup has been compleated ");

  /*********************************************************************************************
  This is just setting up so i can use the OLED screen. This is primarally for debugging so we 
  can test the communication without needining to plug it in to a computer becouse you can only 
  read one serial at a time. 
  *********************************************************************************************/
  /*
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  */
  
  /*********************************************************************************************
  Initalizes the multiplexer I2C, and sets the diffrent pinouts
  *********************************************************************************************/
  if (! aw1.begin(0x58)) { Serial.println("0x58 not found?");while (1) delay(10);} Serial.println("0x58 found!");//0x58 is default adress
  //if (! aw1.begin(0x58)) { Serial.println("0x59 not found?");while (1) delay(10);} Serial.println("0x59 found!");//0x59 (A0 shorted)
  //if (! aw1.begin(0x58)) { Serial.println("0x5A not found?");while (1) delay(10);} Serial.println("0x5A found!");//0x5A (A1 shorted)

  aw1.pinMode(B1PR, OUTPUT);  aw1.pinMode(B1PB, OUTPUT);  aw1.pinMode(B1PG, OUTPUT);  // Board 1
  aw1.pinMode(B2PR, OUTPUT);  aw1.pinMode(B2PB, OUTPUT);  aw1.pinMode(B2PG, OUTPUT);  // Board 2
  aw1.pinMode(B3PR, OUTPUT);  aw1.pinMode(B3PB, OUTPUT);  aw1.pinMode(B3PG, OUTPUT);  // Board 3
  aw1.pinMode(B4PR, OUTPUT);  aw1.pinMode(B4PB, OUTPUT);  aw1.pinMode(B4PG, OUTPUT);  // Board 4
  aw1.pinMode(B5PR, OUTPUT);  aw1.pinMode(B5PB, OUTPUT);  aw1.pinMode(B5PG, OUTPUT);  // Board 5
  aw2.pinMode(B6PR, OUTPUT);  aw2.pinMode(B6PB, OUTPUT);  aw2.pinMode(B6PG, OUTPUT);  // Board 6
  aw2.pinMode(B7PR, OUTPUT);  aw2.pinMode(B7PB, OUTPUT);  aw2.pinMode(B7PG, OUTPUT);  // Board 7
  aw2.pinMode(B8PR, OUTPUT);  aw2.pinMode(B8PB, OUTPUT);  aw2.pinMode(B8PG, OUTPUT);  // Board 8
  aw2.pinMode(B9PR, OUTPUT);  aw2.pinMode(B9PB, OUTPUT);  aw2.pinMode(B9PG, OUTPUT);  // Board 9
  aw2.pinMode(B10PR, OUTPUT); aw2.pinMode(B10PB, OUTPUT); aw2.pinMode(B10PG, OUTPUT); // Board 10
  aw3.pinMode(B11PR, OUTPUT); aw3.pinMode(B11PB, OUTPUT); aw3.pinMode(B11PG, OUTPUT); // Board 11
  aw3.pinMode(B12PR, OUTPUT); aw3.pinMode(B12PB, OUTPUT); aw3.pinMode(B12PG, OUTPUT); // Board 12

  //////////////////////////
  // Set Testing Values
  //////////////////////////
  Serial.println("Giving Fake Values and saving into the structs ");
  for (int i = 0; i < 15; i++)
  {
    myData.id = i;
    boardsStruct[myData.id].x = i;
    boardsStruct[myData.id].y = i;
    boardsStruct[myData.id].z = i;
    boardsStruct[myData.id].isMoving = random(2); // should produce a 0 or 1, 
    String str;
    if(boardsStruct[myData.id].isMoving == 1){str = "TRUE";};
    if(boardsStruct[myData.id].isMoving == 0){str = "FLASE";};
    Serial.printf("X:%d  Y:%d  Z:%d moving? %s \n", boardsStruct[myData.id].x,boardsStruct[myData.id].y,boardsStruct[myData.id].z,  str.c_str() );
  }
  



}

void loop()
{
  //well have to do something like this for the end product
  /***********************************************************
  1) Waits for the sequence
  2) Does the sequence. Tallies the correct and incorect. Notes time between interaction
  3) Sends the data
  4)
  ***********************************************************/
  while (var < (sizeof(sequence)/sizeof(int))) 
  {
    unsigned long startTime = millis(); // gets current time for timeout  
    Serial.println("--------------------------------------------------------");
    Serial.printf("Step # %d wants box #%d moved \n", var,sequence[var]);
    MoveMe(); // indicates which box to move with a blue LEDs
    while (millis() - startTime< timeout);
    {

      // Read the board if you want moved
      // check for movement for others
      //if(movement==True){break;}
    }



    
    
    
    delay(1000);
    int rnd=Imoved();
    CheckifChosenCorectly(rnd,var,sequence);

    

    delay(1000);
    RGB_reset(); delay(500);
    var++;
  }
}

void printOLED()
{
  int i=0; //This will allow for future itterations that we loop between all the boards
  display.clearDisplay(); 
  display.setCursor(0,0); display.print(boardsStruct[i].id +1);
  display.setCursor(0,8); display.print("X");
  display.setCursor(0,16); display.print("Y");
  display.setCursor(0,24); display.print("Z");
  
  display.setCursor(20,8); display.print(boardsStruct[i].x);
  display.setCursor(20,16); display.print(boardsStruct[i].y);
  display.setCursor(20,24); display.print(boardsStruct[i].z);
  if(boardsStruct[i].isMoving)
  {display.setCursor(20,0); display.print(" T ");}
  else
  {display.setCursor(20,0); display.print(" F ");}
  display.display();
}
void MoveMe()
{
  if(sequence[var]==1)  {setBoardColor(1,  LOW, HIGH, LOW); Serial.println("Box 1 set to blue");}
  if(sequence[var]==2)  {setBoardColor(2,  LOW, HIGH, LOW); Serial.println("Box 2 set to blue");}
  if(sequence[var]==3)  {setBoardColor(3,  LOW, HIGH, LOW); Serial.println("Box 3 set to blue");}
  if(sequence[var]==4)  {setBoardColor(4,  LOW, HIGH, LOW); Serial.println("Box 4 set to blue");}
  if(sequence[var]==5)  {setBoardColor(5,  LOW, HIGH, LOW); Serial.println("Box 5 set to blue");}
  if(sequence[var]==6)  {setBoardColor(6,  LOW, HIGH, LOW); Serial.println("Box 6 set to blue");}
  if(sequence[var]==7)  {setBoardColor(7,  LOW, HIGH, LOW); Serial.println("Box 7 set to blue");}
  if(sequence[var]==8)  {setBoardColor(8,  LOW, HIGH, LOW); Serial.println("Box 8 set to blue");}
  if(sequence[var]==9)  {setBoardColor(9,  LOW, HIGH, LOW); Serial.println("Box 9 set to blue");}
  if(sequence[var]==10) {setBoardColor(10, LOW, HIGH, LOW); Serial.println("Box 0 set to blue");}
  if(sequence[var]==11) {setBoardColor(11, LOW, HIGH, LOW); Serial.println("Box 1 set to blue");}
  if(sequence[var]==12) {setBoardColor(12, LOW, HIGH, LOW); Serial.println("Box 2 set to blue");}
}
void MoveMe_Correct()
{
  if(sequence[var]==1)  {setBoardColor(1,  LOW, LOW, HIGH); Serial.println("Box 1 set to Green");}
  if(sequence[var]==2)  {setBoardColor(2,  LOW, LOW, HIGH); Serial.println("Box 2 set to Green");}
  if(sequence[var]==3)  {setBoardColor(3,  LOW, LOW, HIGH); Serial.println("Box 3 set to Green");}
  if(sequence[var]==4)  {setBoardColor(4,  LOW, LOW, HIGH); Serial.println("Box 4 set to Green");}
  if(sequence[var]==5)  {setBoardColor(5,  LOW, LOW, HIGH); Serial.println("Box 5 set to Green");}
  if(sequence[var]==6)  {setBoardColor(6,  LOW, LOW, HIGH); Serial.println("Box 6 set to Green");}
  if(sequence[var]==7)  {setBoardColor(7,  LOW, LOW, HIGH); Serial.println("Box 7 set to Green");}
  if(sequence[var]==8)  {setBoardColor(8,  LOW, LOW, HIGH); Serial.println("Box 8 set to Green");}
  if(sequence[var]==9)  {setBoardColor(9,  LOW, LOW, HIGH); Serial.println("Box 9 set to Green");}
  if(sequence[var]==10) {setBoardColor(10, LOW, LOW, HIGH); Serial.println("Box 0 set to Green");}
  if(sequence[var]==11) {setBoardColor(11, LOW, LOW, HIGH); Serial.println("Box 1 set to Green");}
  if(sequence[var]==12) {setBoardColor(12, LOW, LOW, HIGH); Serial.println("Box 2 set to Green");}
}
void MoveMe_WRONG()
{
  if(sequence[var]==1)  {setBoardColor(1,  HIGH, LOW, LOW); Serial.println("Box 1 set to RED");}
  if(sequence[var]==2)  {setBoardColor(2,  HIGH, LOW, LOW); Serial.println("Box 2 set to RED");}
  if(sequence[var]==3)  {setBoardColor(3,  HIGH, LOW, LOW); Serial.println("Box 3 set to RED");}
  if(sequence[var]==4)  {setBoardColor(4,  HIGH, LOW, LOW); Serial.println("Box 4 set to RED");}
  if(sequence[var]==5)  {setBoardColor(5,  HIGH, LOW, LOW); Serial.println("Box 5 set to RED");}
  if(sequence[var]==6)  {setBoardColor(6,  HIGH, LOW, LOW); Serial.println("Box 6 set to RED");}
  if(sequence[var]==7)  {setBoardColor(7,  HIGH, LOW, LOW); Serial.println("Box 7 set to RED");}
  if(sequence[var]==8)  {setBoardColor(8,  HIGH, LOW, LOW); Serial.println("Box 8 set to RED");}
  if(sequence[var]==9)  {setBoardColor(9,  HIGH, LOW, LOW); Serial.println("Box 9 set to RED");}
  if(sequence[var]==10) {setBoardColor(10, HIGH, LOW, LOW); Serial.println("Box 0 set to RED");}
  if(sequence[var]==11) {setBoardColor(11, HIGH, LOW, LOW); Serial.println("Box 1 set to RED");}
  if(sequence[var]==12) {setBoardColor(12, HIGH, LOW, LOW); Serial.println("Box 2 set to RED");}
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
  int rnd = random(2); Serial.printf("I moved Box number %d \n ", rnd);
  return rnd;
}
  void CheckifChosenCorectly(int rnd,int var, int sequence[])
{
  if (rnd == sequence[var]) {MoveMe_Correct(); Serial.println("The correct one was moved"); delay(1000);}
  if (rnd != sequence[var]) {MoveMe_WRONG(); Serial.println("That was the incorrect movement"); delay(1000);}
}