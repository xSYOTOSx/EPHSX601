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


/*********************************************************************************************
 Setting up the global values to so i can use the printed labels on the board to call
 what digital pins i want to use. This is curently for testing purposes, when i get the multi-
 plexer set up ill have to change how i control the LEDs.
*********************************************************************************************/
int D0=16;int D1=5;int D2=4;int D3=0;int D4=2;int D5=14;int D6=12;int D7=13;int D8=15; 

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
  // Same Shit Diffrent Day
  Serial.begin(115200);
  delay(2000);
  Serial.println("Serial Has Begun");

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(D6, OUTPUT); pinMode(D7, OUTPUT); pinMode(D8, OUTPUT); 
  digitalWrite(D6, LOW); //Green
  digitalWrite(D7, LOW); //Blue
  digitalWrite(D8, LOW); //RED

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

}

void loop()
{
  printLED();
  //printOLED();
  //delay(1000); 
  reset(); 
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

void printLED()
{
  if(boardsStruct[2].isMoving) //True Case
  {
    digitalWrite(D8, LOW);
    digitalWrite(D6, HIGH);
    Serial.println("HIGH");
    delay(1500);
  }
  else
  {
    digitalWrite(D8, HIGH);
    digitalWrite(D6, LOW);
    Serial.println("LOW");
    //delay(500);
  }
}
void reset()
{
  digitalWrite(D6, LOW); //GREEN
  digitalWrite(D7, LOW); //BLUE
  digitalWrite(D8, HIGH); //RED
  Serial.println("reset");
}