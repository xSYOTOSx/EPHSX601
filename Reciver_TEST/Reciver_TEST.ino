/*********************************************************************************************
  This code is heavilly dependent on Rui Santos code on randomnerdtutorials.com. For complete 
  project details visit https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/.
  
  From here orginial code: "Permission is hereby granted, free of charge, to any person 
  obtaining a copy of this software and associated documentation files. The above copyright
  notice and this permission notice shall be included in all copies or substantial portions 
  of the Software."

  Terms
    Trial : One object being moved
    Sequence: a grouping of trials

  Authers: Ryan Coppens & Thresa Kelly
*********************************************************************************************/
// ESP Board MAC Address:  C8:C9:A3:5B:DA:BB
#include <ESP8266WiFi.h> 
#include <espnow.h>
#include <Adafruit_AW9523.h> // for multiplexer

//Box # Color ie B1PR is Box 1 Pin for Red is 1
Adafruit_AW9523 aw1;
uint8_t B1PR = 1;  uint8_t B1PB = 2;  uint8_t B1PG = 3;  // Board 1
uint8_t B2PR = 4;  uint8_t B2PB = 5;  uint8_t B2PG = 6;  // Board 2
uint8_t B3PR = 7;  uint8_t B3PB = 8;  uint8_t B3PG = 9;  // Board 3
uint8_t B4PR = 10; uint8_t B4PB = 11; uint8_t B4PG = 12; // Board 4
Adafruit_AW9523 aw2;
uint8_t B5PR = 13; uint8_t B5PB = 14; uint8_t B5PG = 15;    // Board 5
uint8_t B6PR = 1;   uint8_t B6PB = 2;   uint8_t B6PG = 3;   // Board 6
uint8_t B7PR = 4;   uint8_t B7PB = 5;   uint8_t B7PG = 6;   // Board 7
uint8_t B8PR = 7;   uint8_t B8PB = 8;   uint8_t B8PG = 9;   // Board 8
Adafruit_AW9523 aw3;
uint8_t B9PR = 10;  uint8_t B9PB = 11;  uint8_t B9PG = 12;  // Board 9
uint8_t B10PR = 13; uint8_t B10PB = 14; uint8_t B10PG = 15; // Board 10
uint8_t B11PR = 1;  uint8_t B11PB = 2;  uint8_t B11PG = 3;  // Board 11
uint8_t B12PR = 4;  uint8_t B12PB = 5;  uint8_t B12PG = 6;  // Board 12

// command IDs
const int PING       = 0;
const int TESTLED    = 1; // not implemented in software
const int BATTERY    = 2; // not implemented in software
const int CALIBRATE  = 3; // not implemented in software
const int NTRIALS    = 4;
const int TRIAL      = 5;
const int SEPARATION = 6;
const int TIMEOUT    = 7;
const int SAMPLERATE = 8;
const int STREAM     = 9;

/////////////////////////////////////////////////////////////////////////
// number of objects
const int NOBJECTS   = 12;
int sequence[10] = {1, 1, 2, 2, 2,1,1,2,1,1};
unsigned long timeout = 5000;
unsigned long previousMillis = 0;
int i =0;//global counter. May have a better way if implementing this

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
struct_message boardsStruct[15] = {board1, board2,board3,board4,board5, board6,board7,board8,board9, board10,board11,board12,board13, board14,board15};

void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len)
{
  char macStr[18];
  //Serial.print("Packet received from: ");
  //snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.print(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  //Serial.printf("Board ID: %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id].id = myData.id;
  boardsStruct[myData.id].x = myData.x;
  boardsStruct[myData.id].y = myData.y;
  boardsStruct[myData.id].z = myData.z;
  boardsStruct[myData.id].m = myData.m;
  boardsStruct[myData.id].isMoving = myData.isMoving;
  Serial.printf("ID:%d X:%f Y:%f Z:%f m:%f T/F:%d \n",
                  boardsStruct[myData.id].id,
                  boardsStruct[myData.id].x,
                  boardsStruct[myData.id].y,
                  boardsStruct[myData.id].z,
                  boardsStruct[myData.id].m,
                  boardsStruct[myData.id].isMoving);
  //Serial.println();
}

void setup() 
{
  Serial.begin(115200); while(! Serial);Serial.println("Hello World! Serial Has Begun");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Device set as a Wi-Fi Station");
  if (esp_now_init() != 0) {Serial.println("Error initializing ESP-NOW"); return; }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Setup has been compleated ");

}

void loop()
{
  //for(int i = 1; i <4 ; i++) 
  { int i=1;
    //Serial.printf("ID:%d X:%f Y:%f Z:%f m:%f T/f: %b \n",boardsStruct[i].id,boardsStruct[i].x,boardsStruct[i].y,boardsStruct[i].z,boardsStruct[i].m,boardsStruct[i].isMoving);
  }
  


  
}
