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



// data packet read from software 
struct packet 
{
  int commandNumber;
  int objectID;
  int data;
};

//  holds character array of hex numbers with array size
struct hexString 
{
  char * hex;
  int    size;
};


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
  //Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID: %u: %u bytes\n", myData.id, len);
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
  
  //Initalizes the multiplexer I2C
  //if (! aw1.begin(0x58)) { Serial.println("0x58 not found?");while (1) delay(10);} Serial.println("0x58 found!");//0x58 is default adress
  //if (! aw1.begin(0x58)) { Serial.println("0x59 not found?");while (1) delay(10);} Serial.println("0x59 found!");//0x59 (A0 shorted)
  //if (! aw1.begin(0x58)) { Serial.println("0x5A not found?");while (1) delay(10);} Serial.println("0x5A found!");//0x5A (A1 shorted)

  //Defines pins on the multiplexer
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
}

void loop()
{
  //DoCommand(ReadCommand());


  
}
void DoCommand(struct packet currentCommand)
{
  // easy access of struct parts
  int cmd  = currentCommand.commandNumber;
  int id   = currentCommand.objectID;
  int data = currentCommand.data;

  // choose function using command number 
  switch(cmd) {
    case PING:
      Ping();  
      break;
    case TESTLED:
      TestLED(id,data);
      break;
    case BATTERY:
      Battery(id);
      break;
    case CALIBRATE:
      Calibrate(id);
      break;
    case NTRIALS:
      NTrials(data);
      break;
    case TRIAL:
      Trial(id,data);
      break;      
    case SEPARATION:
      Separation(data);
      break;
    case TIMEOUT:
      Timeout(data);
      break;
    case SAMPLERATE:
      SampleRate(data);
      break;
    case STREAM:
      //Stream(data);
      TestSequence();
      break;
    default:
      NoCommand();
  }
}

void TestSequence()
{
  while (i < (sizeof(sequence)/sizeof(int)))
  {
    unsigned long startTime = millis(); // gets current time for timeout  
    if((millis() - startTime) > SAMPLERATE)
    {
      // Write to computer
    }

    MoveMe(); // indicates which box to move with a blue LEDs
    
    bool isMoving = false; // using to set values to allow breaking of loops
    bool OtherBoxMoving = false;
    while ((millis() - startTime) < timeout) //Timeout while checking, breaks if moved
    {
      if (boardsStruct[sequence[i]].isMoving) {isMoving = true; break; } //Checks if commanded is moving 
      for(int j=1; j<13;j++) // checks rest of boxes
      {
        if (j == sequence[i]){ continue;} // skip curent box
        if (boardsStruct[j].isMoving) { OtherBoxMoving = true; break;} 
      }
      if(OtherBoxMoving){break;}
    }
    
    if (isMoving) {MoveMe_Correct();delay(750);} // Correct LED Responce
    else if(OtherBoxMoving ) // Wrong LED Responce
    {
        for(int j=0; j<5; j++)
        {
          MoveMe_WRONG();delay(250);RGB_reset(); delay(250); // Allows for Flashing of RED LED
        }
    }
    else {MoveMe_WRONG();delay(1000);} // Timeout LED Reponce
    RGB_reset(); 
    //delay(separation);
    i++;
  }
}

bool WritePacket(struct packet p)
{  
  // check packet inputs 
  if( (p.commandNumber > STREAM) || // check that command number is valid 
      (p.commandNumber < PING) ||
      (p.objectID > NOBJECTS) ||    // check for valid ID 
      (p.objectID < 0)
  ){
    return(false);
  }

  // convert to hex
  struct hexString cmd  = IntToHexString(p.commandNumber);
  struct hexString id   = IntToHexString(p.objectID);
  struct hexString data = IntToHexString(p.data);

  // check char* sizes
  if( (cmd.size  > 1) ||
      (id.size   > 1) ||
      (data.size > 4)
  ){
    return(false);
  }  
  
  // initialize packet of 8 bytes
  char command[] = "00000000";

  // set start/stop flags
  command[0] = 0x02; // STX
  command[7] = 0x03; // ETX
  
  // write command info 
  command[1] = cmd.hex[0];
  command[2] = id.hex[0];
  if(data.size == 4){
    command[3] = data.hex[0];
    command[4] = data.hex[1];
    command[5] = data.hex[2];
    command[6] = data.hex[3];
  } else if(data.size == 3){
    command[4] = data.hex[0];
    command[5] = data.hex[1];
    command[6] = data.hex[2];    
  }else if(data.size == 2){
    command[5] = data.hex[0];
    command[6] = data.hex[1];       
  } else { // data.size == 1
    command[6] = data.hex[0];     
  }

  // write to computer 
  Serial.write(command);  

  // delete pointers to prevent memory leak
  delete[] cmd.hex;
  delete[] id.hex;
  delete[] data.hex;
  
  // return true for successful write
  return(true);
}

struct hexString IntToHexString(uint n)
{
  struct hexString hs; 
  if(n == 0)
  {
    hs.size = 1;    
  }
  else
  {
    // calculate number of digits needed to represent number in hex
    hs.size = floor( log(n)/log(16) ) + 1;       
  }
  // convert n to hex string 
  hs.hex = new char[hs.size];
  sprintf(hs.hex, "%X", n);   
  return(hs);
}

struct packet ReadCommand()
{
  // init strings
  char b0_stx[]     = "0"; // note: string terminates with '\0'
  char b1_cmd[]     = "0";
  char b2_id[]      = "0";
  char b3456_data[] = "0000";
  char b7_etx[]     = "0";

  // read until STX found, which marks the start of a data packet
  while(true)
  {
    // wait for data 
    if(Serial.available() > 0)
    {
      // read a byte
      b0_stx[0] = Serial.read();
      // check if byte is STX
      if(b0_stx[0] == 0x02)
      {
        break;
      }
    }        
  }   
  
  // wait for full packet to be available 
  while(Serial.available() < 7){
    delay(0.5);
  }

  // read next bytes  
  b1_cmd[0]   = Serial.read();
  b2_id[0]    = Serial.read();
  b3456_data[0] = Serial.read();
  b3456_data[1] = Serial.read();
  b3456_data[2] = Serial.read();
  b3456_data[3] = Serial.read();
  b7_etx[0]   = Serial.read();

  // verify that last byte is ETX, which ends the packet
  if( b7_etx[0] != 0x03)
  {
    // bad packet, try to read again 
    return(ReadCommand());
  }

  // convert hex characters into integers and add to struct 
  struct packet p;
  p.commandNumber = HexStringToInt(b1_cmd);
  p.objectID      = HexStringToInt(b2_id);
  p.data          = HexStringToInt(b3456_data);

  return(p);
}

int HexStringToInt(char str[])
{
  // convert unsigned hex string into integer
  return (int) strtoul(str, 0, 16);
}

void Ping()
{
  struct packet p;
  p.commandNumber = PING;
  p.objectID = 0;
  p.data = 0;
  bool status = WritePacket(p);
}

void TestLED(int id, int time_ms)
{
  
}

void Battery(int id)
{
  
}

void Calibrate(int id)
{

}

void NTrials(int numberOfTrials)
{
  
}

void Trial(int id, int trialNumber)
{
  
}

void Separation(int time_ms)
{
  separation = time_ms; // time between trials
}

void Timeout(int time_ms)
{
  timeout = time_ms;

  struct packet p;
  p.commandNumber = TIMEOUT;
  p.objectID =0;
  p.data = 0;
  bool status = WritePacket(p);
}

void SampleRate(int frequency_Hz)
{
  
}

void Stream(int flag)
{
  
}

void NoCommand()
{
  Serial.print(".")
}

void MoveMe()
{
  if(sequence[i]==1)  {setBoardColor(1,  LOW, HIGH, LOW); Serial.println("Box 1 set to blue");}
  if(sequence[i]==2)  {setBoardColor(2,  LOW, HIGH, LOW); Serial.println("Box 2 set to blue");}
  if(sequence[i]==3)  {setBoardColor(3,  LOW, HIGH, LOW); Serial.println("Box 3 set to blue");}
  if(sequence[i]==4)  {setBoardColor(4,  LOW, HIGH, LOW); Serial.println("Box 4 set to blue");}
  if(sequence[i]==5)  {setBoardColor(5,  LOW, HIGH, LOW); Serial.println("Box 5 set to blue");}
  if(sequence[i]==6)  {setBoardColor(6,  LOW, HIGH, LOW); Serial.println("Box 6 set to blue");}
  if(sequence[i]==7)  {setBoardColor(7,  LOW, HIGH, LOW); Serial.println("Box 7 set to blue");}
  if(sequence[i]==8)  {setBoardColor(8,  LOW, HIGH, LOW); Serial.println("Box 8 set to blue");}
  if(sequence[i]==9)  {setBoardColor(9,  LOW, HIGH, LOW); Serial.println("Box 9 set to blue");}
  if(sequence[i]==10) {setBoardColor(10, LOW, HIGH, LOW); Serial.println("Box 0 set to blue");}
  if(sequence[i]==11) {setBoardColor(11, LOW, HIGH, LOW); Serial.println("Box 1 set to blue");}
  if(sequence[i]==12) {setBoardColor(12, LOW, HIGH, LOW); Serial.println("Box 2 set to blue");}
}
void MoveMe_Correct()
{
  if(sequence[i]==1)  {setBoardColor(1,  LOW, LOW, HIGH); Serial.println("Box 1 set to Green");}
  if(sequence[i]==2)  {setBoardColor(2,  LOW, LOW, HIGH); Serial.println("Box 2 set to Green");}
  if(sequence[i]==3)  {setBoardColor(3,  LOW, LOW, HIGH); Serial.println("Box 3 set to Green");}
  if(sequence[i]==4)  {setBoardColor(4,  LOW, LOW, HIGH); Serial.println("Box 4 set to Green");}
  if(sequence[i]==5)  {setBoardColor(5,  LOW, LOW, HIGH); Serial.println("Box 5 set to Green");}
  if(sequence[i]==6)  {setBoardColor(6,  LOW, LOW, HIGH); Serial.println("Box 6 set to Green");}
  if(sequence[i]==7)  {setBoardColor(7,  LOW, LOW, HIGH); Serial.println("Box 7 set to Green");}
  if(sequence[i]==8)  {setBoardColor(8,  LOW, LOW, HIGH); Serial.println("Box 8 set to Green");}
  if(sequence[i]==9)  {setBoardColor(9,  LOW, LOW, HIGH); Serial.println("Box 9 set to Green");}
  if(sequence[i]==10) {setBoardColor(10, LOW, LOW, HIGH); Serial.println("Box 0 set to Green");}
  if(sequence[i]==11) {setBoardColor(11, LOW, LOW, HIGH); Serial.println("Box 1 set to Green");}
  if(sequence[i]==12) {setBoardColor(12, LOW, LOW, HIGH); Serial.println("Box 2 set to Green");}
}
void MoveMe_WRONG()
{
  if(sequence[i]==1)  {setBoardColor(1,  HIGH, LOW, LOW); Serial.println("Box 1 set to RED");}
  if(sequence[i]==2)  {setBoardColor(2,  HIGH, LOW, LOW); Serial.println("Box 2 set to RED");}
  if(sequence[i]==3)  {setBoardColor(3,  HIGH, LOW, LOW); Serial.println("Box 3 set to RED");}
  if(sequence[i]==4)  {setBoardColor(4,  HIGH, LOW, LOW); Serial.println("Box 4 set to RED");}
  if(sequence[i]==5)  {setBoardColor(5,  HIGH, LOW, LOW); Serial.println("Box 5 set to RED");}
  if(sequence[i]==6)  {setBoardColor(6,  HIGH, LOW, LOW); Serial.println("Box 6 set to RED");}
  if(sequence[i]==7)  {setBoardColor(7,  HIGH, LOW, LOW); Serial.println("Box 7 set to RED");}
  if(sequence[i]==8)  {setBoardColor(8,  HIGH, LOW, LOW); Serial.println("Box 8 set to RED");}
  if(sequence[i]==9)  {setBoardColor(9,  HIGH, LOW, LOW); Serial.println("Box 9 set to RED");}
  if(sequence[i]==10) {setBoardColor(10, HIGH, LOW, LOW); Serial.println("Box 0 set to RED");}
  if(sequence[i]==11) {setBoardColor(11, HIGH, LOW, LOW); Serial.println("Box 1 set to RED");}
  if(sequence[i]==12) {setBoardColor(12, HIGH, LOW, LOW); Serial.println("Box 2 set to RED");}
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
  void CheckifChosenCorectly(int rnd,int var, int sequence[])
{
  if (rnd == sequence[var]) {MoveMe_Correct(); Serial.println("The correct one was moved"); delay(1000);}
  if (rnd != sequence[var]) {MoveMe_WRONG(); Serial.println("That was the incorrect movement"); delay(1000);}
}