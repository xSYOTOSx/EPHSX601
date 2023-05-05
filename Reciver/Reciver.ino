#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h> 
#include <espnow.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include <Adafruit_AW9523.h> // for multiplexer
#include <arduino-timer.h> // https://github.com/contrem/arduino-timer // Copyright (c) 2018, Michael Contreras

// ======================================
// Created by:  Thresa Kelly
// Email:       ThresaKelly133@gmail.com
// ======================================

////////////////////////////////////////////////////////////////////////////////////
// vvv Display screen stuff vvv
////////////////////////////////////////////////////////////////////////////////////
// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels
// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

////////////////////////////////////////////////////////////////////////////////////
// vvv arduino-timer stuff vvv
////////////////////////////////////////////////////////////////////////////////////

// timer 
auto timer = timer_create_default(); // create a timer with default settings

////////////////////////////////////////////////////////////////////////////////////
// vvv my globals vvv
////////////////////////////////////////////////////////////////////////////////////

//// structs ////

// data packet read from software 
struct packet {
  int commandNumber;
  int objectID;
  int data;
};

//  holds character array of hex numbers with array size
struct hexString {
  char * hex;
  int    size;
};

//// constants ////

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

// number of objects
const int NOBJECTS   = 11; //12;

// constant for bad no value or bad value
const int NOVALUE    = -1;

// system states 
const int MODE_INIT   = 0; // setting experiment parameters 
const int MODE_STREAM = 1; // experiment running

//// variables ////

// experiment state
int   _state           = NOVALUE;

// experiment parameters
int   _numberOfTrials  = NOVALUE;
int * _trialSequence   = nullptr;
int   _separation_ms   = NOVALUE;
int   _timeout_ms      = NOVALUE; 
int   _sampleRate_Hz   = NOVALUE;

// current experiment status
int   _currentTrial    = NOVALUE; // trial number (1 to _numberOfTrials)
int   _currentObject   = NOVALUE; // ID of object that was moved

////////////////////////////////////////////////////////////////////////////////////
// vvv Arduino functions vvv
////////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  
  SystemSetup();

  
  // // setup display
  // if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  //   Serial.println("SSD1306 allocation failed");
  //   for(;;);
  // }
  // delay(500);

  // TestDisplay();
  delay(500);
}

// put your main code here, to run repeatedly:
void loop() 
{
  // read one data packet from computer and do the command 
  if(Serial.available() > 0){
    bool status = DoCommand(ReadCommand());  
  }
  
  // experiment running    
  if(_state == MODE_STREAM){
    timer.tick();
    TestSequence();
  }
}

////////////////////////////////////////////////////////////////////////////////////
// vvv timer vvv
////////////////////////////////////////////////////////////////////////////////////

bool TimerEvent_WriteSTREAM(void *)
{
  bool keepTimerActive = true;

  // check for valid parameters
  if(_currentTrial < 1 || _state != MODE_STREAM ){
    keepTimerActive = false;
  }
  else{
    // write data to computer 
    WritePacket(STREAM, _currentObject, _currentTrial); 
  }

  return keepTimerActive;
}

// ////////////////////////////////////////////////////////////////////////////////////
// // vvv screen control vvv
// ////////////////////////////////////////////////////////////////////////////////////

// void ClearScreen()
// {
//   display.clearDisplay();
//   display.setCursor(0,20);
//   display.display();    
// }

// void TestDisplay()
// {
//   display.clearDisplay();
//   display.setTextSize(1);             
//   display.setTextColor(WHITE);        
//   display.setCursor(0,20);             
//   display.println("Hello, world!");
//   display.display();
// }

// void DisplayPacket(int commandNumber, int objectID, int data)
// {
//   display.clearDisplay();
//   display.setCursor(0,20);             
//   display.println(commandNumber);
//   display.println(objectID);
//   display.println(data);
//   display.display();
// }

// void DisplayPacket(char * commandNumber, char * objectID, char * data)
// {
//   display.clearDisplay();
//   display.setCursor(0,20);             
//   display.println(commandNumber);
//   display.println(objectID);
//   display.println(data);
//   display.display();
// }

// void DisplayChar(char * pkt)
// {
//   display.clearDisplay();
//   display.setCursor(0,20);             
//   display.println(pkt);
//   display.display();
// }

// void DisplayInt(int n)
// {
//   display.clearDisplay();
//   display.setCursor(0,20);             
//   display.println(n);
//   display.display();  
// }

////////////////////////////////////////////////////////////////////////////////////
// vvv data packets vvv
////////////////////////////////////////////////////////////////////////////////////

bool WritePacket(struct packet p)
{
  return(WritePacket(p.commandNumber, p.objectID, p.data));
}

bool WritePacket(int commandNumber, int objectID, int data)
{  
  // check packet inputs 
  if( ( commandNumber > STREAM   ) || // check that command number is valid 
      ( commandNumber < PING     ) ||
      ( objectID      > NOBJECTS ) || // check for valid ID 
      ( objectID      < 0        )
  ){
    return(false);
  }

  // convert to hex
  struct hexString cmdHex  = IntToHexString(commandNumber);
  struct hexString idHex   = IntToHexString(objectID);
  struct hexString dataHex = IntToHexString(data);

  // check char* sizes
  if( (cmdHex.size  > 1) ||
      (idHex.size   > 1) ||
      (dataHex.size > 4)
  ){
    // delete pointers to prevent memory leak
    delete[] cmdHex.hex;
    delete[] idHex.hex;
    delete[] dataHex.hex;
    return(false);
  }  
  
  // initialize packet of 8 bytes
  char command[] = "00000000";

  // set start/stop flags
  command[0] = 0x02; // STX
  command[7] = 0x03; // ETX
  
  // write command info 
  command[1] = cmdHex.hex[0];
  command[2] = idHex.hex[0];
  if(dataHex.size == 4){
    command[3] = dataHex.hex[0];
    command[4] = dataHex.hex[1];
    command[5] = dataHex.hex[2];
    command[6] = dataHex.hex[3];
  } else if(dataHex.size == 3){
    command[4] = dataHex.hex[0];
    command[5] = dataHex.hex[1];
    command[6] = dataHex.hex[2];    
  }else if(dataHex.size == 2){
    command[5] = dataHex.hex[0];
    command[6] = dataHex.hex[1];       
  } else { // data.size == 1
    command[6] = dataHex.hex[0];     
  }

  // write to computer 
  Serial.write(command);

  // delete pointers to prevent memory leak
  delete[] cmdHex.hex;
  delete[] idHex.hex;
  delete[] dataHex.hex;
  
  // return true for successful write
  return(true);
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
  while(true){
    // wait for data 
    if(Serial.available() > 0){
      // read a byte
      b0_stx[0] = Serial.read();
      // check if byte is STX
      if(b0_stx[0] == 0x02){
        break;
      }
    }        
  }   
  
  // wait for full packet to be available 
  while(Serial.available() < 7){
    delayMicroseconds(200); // 0.2 ms
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
  if( b7_etx[0] != 0x03){
    // bad packet, try to read again 
    return(ReadCommand());
  }

  // convert hex characters into integers and add to struct 
  struct packet p;
  p.commandNumber = HexStringToInt(b1_cmd);
  p.objectID      = HexStringToInt(b2_id);
  p.data          = HexStringToInt(b3456_data);

  // return packet 
  return(p);
}

int HexStringToInt(char str[])
{
  // convert unsigned hex string into integer
  return (int) strtoul(str, 0, 16);
}

struct hexString IntToHexString(uint n)
{
  // init struct 
  struct hexString hs; 
  
  // check edge case
  if(n == 0){
    // only 1 digit needed for 0 hex
    hs.size = 1;    
  }
  else{
    // calculate number of digits needed to represent number in hex
    hs.size = floor( log(n)/log(16) ) + 1;       
  }

  // convert n to hex string 
  hs.hex = new char[hs.size];
  sprintf(hs.hex, "%X", n);   // BUG: RESETS EVERYTHING WHEN TRYING TO CONVERT 12 TO C
  
  // return hex string of n   
  return(hs);
}

////////////////////////////////////////////////////////////////////////////////////
// vvv run commands vvv
////////////////////////////////////////////////////////////////////////////////////

bool DoCommand(struct packet currentCommand)
{
  // true when command was successful, false if failed
  bool status = false;  

  // easy access of struct parts
  int cmd  = currentCommand.commandNumber;
  int id   = currentCommand.objectID;
  int data = currentCommand.data;

  // choose function using command number 
  switch(cmd) {
    case PING:
      status = Ping();  
      break;
    case TESTLED:
      status = TestLED(id,data);  // not implemented in software
      break;
    case BATTERY:
      status = Battery(id);       // not implemented in software
      break;
    case CALIBRATE:
      status = Calibrate(id);     // not implemented in software
      break;
    case NTRIALS:
      status = NTrials(data);
      break;
    case TRIAL:
      status = Trial(id,data);
      break;      
    case SEPARATION:
      status = Separation(data);
      break;
    case TIMEOUT:
      status = Timeout(data);
      break;
    case SAMPLERATE:
      status = SampleRate(data);
      break;
    case STREAM:
      status = Stream(data);
      break;
    default:
      status = NoCommand();
  }

  return(status);  
}

bool Ping()
{
  // true for successful command implementation, false otherwise
  bool status = WritePacket(PING,0,0);
  return(status);
}

bool TestLED(int id, int time_ms)
{
  // not implemented
  return(false); // temp
}

bool Battery(int id)
{
  // not implemented
  return(false); // temp
}

bool Calibrate(int id)
{
  // not implemented 
  return(false); // temp
}

bool NTrials(int numberOfTrials)
{
  // true for successful command implementation, false otherwise
  bool status = false;

  if(numberOfTrials > 0){
    // set number of trials globally
    _numberOfTrials = numberOfTrials;
    
    // clear out old array
    if(_trialSequence != nullptr){
      delete[] _trialSequence;
      _trialSequence = nullptr;
    }  
    
    // initalize sequence array of size _trialSequence and fill with -1s
    _trialSequence = new int[numberOfTrials]; 
    for(int i=0; i<numberOfTrials; i++){
      _trialSequence[i] = NOVALUE;
    } 

    // write back command
    status = WritePacket(NTRIALS,0,_numberOfTrials);   
  }
  // else { bad input, do not write back to software }
  return(status);
}

bool Trial(int id, int trialNumber)
{
  // true for successful command implementation, false otherwise
  bool status = false;
  
  if(_trialSequence != nullptr){
    // set trial 
    _trialSequence[trialNumber-1] = id;

    // write back command
    status = WritePacket(TRIAL,_trialSequence[trialNumber-1],trialNumber);    
  }
  // else { need NTrials first, do not write back to software }
  return(status);
}

bool Separation(int time_ms)
{
  // true for successful command implementation, false otherwise
  bool status = false;

  if(time_ms > 0){
    // set time 
    _separation_ms = time_ms;
    // write back command
    bool status = WritePacket(SEPARATION,0,_separation_ms);
  }
  // else { bad input, do not write back to software }
  return(status);
}

bool Timeout(int time_ms)
{
  // true for successful command implementation, false otherwise
  bool status = false;
  
  if(time_ms > 0){
    // set time
    _timeout_ms = time_ms;
    // write back command
    bool status = WritePacket(TIMEOUT,0,_timeout_ms);    
  }
  // else { bad input, do not write back to software }
  return(status);
}

bool SampleRate(int frequency_Hz)
{
  // true for successful command implementation, false otherwise
  bool status = false;
  
  if(frequency_Hz > 0){
    // set frequency 
    _sampleRate_Hz = frequency_Hz;
    // write back command
    bool status = WritePacket(SAMPLERATE,0,_sampleRate_Hz); 
  }
  // else { bad input, do not write back to software }
  return(status);
}

bool Stream(int flag)
{
  // true for successful command implementation, false otherwise
  bool status = false;
  
  if(flag == 0){
    // stop experiment
    _state = MODE_INIT;  
    // reset experiment variables
    _currentTrial  = NOVALUE;
    _currentObject = NOVALUE;
  }
  else if(flag == 1){
    // start experiment 
    _state = MODE_STREAM;
    // reset experiment variables
    _currentTrial  = 1;
    _currentObject = 0;
    // start timer 
    int dt_ms = (1.0 / _sampleRate_Hz) * 1000; // (1/[Hz]) = [s] // [s] * 1000[ms]/[s] = [ms]
    timer.every(dt_ms, TimerEvent_WriteSTREAM);
  }
  else{
    return(status);    
  }
  
  status = WritePacket(STREAM,0,_state); 
  return(status); 
}

bool NoCommand()
{
  return(false);
}


////////////////////////////////////////////////////////////////////////////////////
// ======================================
// Created by:  Ryan Coppens
// Email:       CoppensRyan@gmail.com
// ======================================
////////////////////////////////////////////////////////////////////////////////////

//Box # Color ie B1PR is Box 1 Pin for Red is 1
Adafruit_AW9523 aw1;
uint8_t B1PR = 1; uint8_t B1PG = 2; uint8_t B1PB = 3;   // Board 1
uint8_t B2PR = 4; uint8_t B2PG =5; uint8_t B2PB = 6;   // Board 2
uint8_t B3PR = 7; uint8_t B3PG = 8; uint8_t B3PB = 9;   // Board 3
uint8_t B4PR = 10;uint8_t B4PG = 11; uint8_t B4PB = 12;  // Board 4
Adafruit_AW9523 aw2;
uint8_t B5PR = 0;   uint8_t B5PG = 1;   uint8_t B5PB = 2;   // Board 5
uint8_t B6PR = 3;   uint8_t B6PG = 4;   uint8_t B6PB = 5;   // Board 6
uint8_t B7PR = 6;   uint8_t B7PG = 7;   uint8_t B7PB = 8;   // Board 7
uint8_t B8PR = 14;  uint8_t B8PG = 15;  uint8_t B8PB = 13;  // Board 8
Adafruit_AW9523 aw3;
uint8_t B9PR =  0;   uint8_t B9PG =   1; uint8_t B9PB =   9;   // Board 9
uint8_t B10PR = 3;   uint8_t B10PG =  4; uint8_t B10PB =  5;  // Board 10
uint8_t B11PR = 6;   uint8_t B11PG =  7; uint8_t B11PB =  8;   // Board 11
uint8_t B12PR = 15;   uint8_t B12PG = 13; uint8_t B12PB = 14;   // Board 12

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
  // char macStr[18];
  // Serial.print("Packet received from: ");
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  // Serial.printf("Board ID: %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id].id = myData.id;
  boardsStruct[myData.id].x = myData.x;
  boardsStruct[myData.id].y = myData.y;
  boardsStruct[myData.id].z = myData.z;
  boardsStruct[myData.id].m = myData.m;
  boardsStruct[myData.id].isMoving = myData.isMoving;
  // Serial.printf("ID:%d X:%f Y:%f Z:%f m:%f T/F:%d \n",
  //               boardsStruct[myData.id].id,
  //               boardsStruct[myData.id].x,
  //               boardsStruct[myData.id].y,
  //               boardsStruct[myData.id].z,
  //               boardsStruct[myData.id].m,
  //               boardsStruct[myData.id].isMoving);
  //Serial.println();
}

void SystemSetup() 
{
  Serial.begin(115200); while(! Serial); //Serial.println("Hello World! Serial Has Begun");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Serial.println("Device set as a Wi-Fi Station");
  if (esp_now_init() != 0) {/*Serial.println("Error initializing ESP-NOW");*/ return; }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  // Serial.println("Setup has been compleated ");
  
  //Initalizes the multiplexer I2C
  if (! aw1.begin(0x58)) { /*Serial.println("0x58 not found?");*/while (1) delay(10);} /*Serial.println("0x58 found!");*///0x58 is default adress
  if (! aw2.begin(0x59)) { /*Serial.println("0x59 not found?");*/while (1) delay(10);} /*Serial.println("0x59 found!");*///0x59 (A0 shorted)
  if (! aw3.begin(0x5A)) { /*Serial.println("0x5A not found?");*/while (1) delay(10);} /*Serial.println("0x5A found!");*///0x5A (A1 shorted)

  //Defines pins on the multiplexer
  for(int i=0; i<=15; i++)
  {
    aw1.pinMode(i, OUTPUT);
    aw2.pinMode(i, OUTPUT);
    aw3.pinMode(i, OUTPUT);
  }
  
  RGB_test();
  sensor_test();
}

void RGB_test()
{
  RGB_reset();
  for(int i=1; i<=12; i++)
  {
    for(int j=1; j<=3; j++)
    {
      if(j=1){setBoardColor(i, HIGH, LOW, LOW);  delay(100); RGB_reset();} //red
      if(j=2){setBoardColor(i, LOW,  LOW, HIGH); delay(100); RGB_reset();} //green
      if(j=3){setBoardColor(i, LOW, HIGH, LOW);  delay(100); RGB_reset();} //blue
    }
  }
}

void sensor_test()
{
  RGB_reset(); 
  
  for(int i=1; i<=12; i++)
    {
      setBoardColor(i, LOW, HIGH, LOW); //b
      while(!boardsStruct[i].isMoving)
      {
       delay(1);
      }
      setBoardColor(i, LOW,  LOW, HIGH); 
      delay(500); RGB_reset(); //g
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

void TestSequence()
{
  while (_currentTrial <= _numberOfTrials)
  {
    unsigned long startTime = millis(); // gets current time for timeout  
    timer.tick();
    MoveMe(); // indicates which box to move with a blue LEDs
    
    bool isMoving = false; // using to set values to allow breaking of loops
    bool OtherBoxMoving = false;
    while ((millis() - startTime) < _timeout_ms) //Timeout while checking, breaks if moved
    {
      if (boardsStruct[ _trialSequence[_currentTrial-1] ].isMoving) {
        isMoving = true; 
        _currentObject = _trialSequence[_currentTrial-1];
        break; 
      } //Checks if commanded is moving 
      
      for(int j=1; j<=NOBJECTS;j++) // checks rest of boxes
      {
        if (j == _trialSequence[_currentTrial-1]){ continue;} // skip curent box
        if (boardsStruct[j].isMoving) { 
          OtherBoxMoving = true; 
          _currentObject = j;
          break;
        } 
        if(j == NOBJECTS){
          _currentObject = 0;          
        }
      }
      if(OtherBoxMoving){break;}
      timer.tick();
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
    _currentTrial++;
  }
}

void MoveMe()
{
  if(_trialSequence == nullptr) {return;}

  if(_trialSequence[_currentTrial-1]==1)  {setBoardColor(1,  LOW, HIGH, LOW); /*Serial.println("Box 1 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==2)  {setBoardColor(2,  LOW, HIGH, LOW); /*Serial.println("Box 2 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==3)  {setBoardColor(3,  LOW, HIGH, LOW); /*Serial.println("Box 3 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==4)  {setBoardColor(4,  LOW, HIGH, LOW); /*Serial.println("Box 4 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==5)  {setBoardColor(5,  LOW, HIGH, LOW); /*Serial.println("Box 5 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==6)  {setBoardColor(6,  LOW, HIGH, LOW); /*Serial.println("Box 6 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==7)  {setBoardColor(7,  LOW, HIGH, LOW); /*Serial.println("Box 7 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==8)  {setBoardColor(8,  LOW, HIGH, LOW); /*Serial.println("Box 8 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==9)  {setBoardColor(9,  LOW, HIGH, LOW); /*Serial.println("Box 9 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==10) {setBoardColor(10, LOW, HIGH, LOW); /*Serial.println("Box 0 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==11) {setBoardColor(11, LOW, HIGH, LOW); /*Serial.println("Box 1 set to blue");*/}
  if(_trialSequence[_currentTrial-1]==12) {setBoardColor(12, LOW, HIGH, LOW); /*Serial.println("Box 2 set to blue");*/}
}

void MoveMe_Correct()
{
  if(_trialSequence == nullptr) {return;}

  if(_trialSequence[_currentTrial-1]==1)  {setBoardColor(1,  LOW, LOW, HIGH); /*Serial.println("Box 1 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==2)  {setBoardColor(2,  LOW, LOW, HIGH); /*Serial.println("Box 2 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==3)  {setBoardColor(3,  LOW, LOW, HIGH); /*Serial.println("Box 3 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==4)  {setBoardColor(4,  LOW, LOW, HIGH); /*Serial.println("Box 4 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==5)  {setBoardColor(5,  LOW, LOW, HIGH); /*Serial.println("Box 5 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==6)  {setBoardColor(6,  LOW, LOW, HIGH); /*Serial.println("Box 6 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==7)  {setBoardColor(7,  LOW, LOW, HIGH); /*Serial.println("Box 7 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==8)  {setBoardColor(8,  LOW, LOW, HIGH); /*Serial.println("Box 8 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==9)  {setBoardColor(9,  LOW, LOW, HIGH); /*Serial.println("Box 9 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==10) {setBoardColor(10, LOW, LOW, HIGH); /*Serial.println("Box 0 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==11) {setBoardColor(11, LOW, LOW, HIGH); /*Serial.println("Box 1 set to Green")*/;}
  if(_trialSequence[_currentTrial-1]==12) {setBoardColor(12, LOW, LOW, HIGH); /*Serial.println("Box 2 set to Green")*/;}
}

void MoveMe_WRONG()
{
  if(_trialSequence == nullptr) {return;}
  
  if(_trialSequence[_currentTrial-1]==1)  {setBoardColor(1,  HIGH, LOW, LOW); /*Serial.println("Box 1 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==2)  {setBoardColor(2,  HIGH, LOW, LOW); /*Serial.println("Box 2 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==3)  {setBoardColor(3,  HIGH, LOW, LOW); /*Serial.println("Box 3 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==4)  {setBoardColor(4,  HIGH, LOW, LOW); /*Serial.println("Box 4 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==5)  {setBoardColor(5,  HIGH, LOW, LOW); /*Serial.println("Box 5 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==6)  {setBoardColor(6,  HIGH, LOW, LOW); /*Serial.println("Box 6 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==7)  {setBoardColor(7,  HIGH, LOW, LOW); /*Serial.println("Box 7 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==8)  {setBoardColor(8,  HIGH, LOW, LOW); /*Serial.println("Box 8 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==9)  {setBoardColor(9,  HIGH, LOW, LOW); /*Serial.println("Box 9 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==10) {setBoardColor(10, HIGH, LOW, LOW); /*Serial.println("Box 0 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==11) {setBoardColor(11, HIGH, LOW, LOW); /*Serial.println("Box 1 set to RED");*/}
  if(_trialSequence[_currentTrial-1]==12) {setBoardColor(12, HIGH, LOW, LOW); /*Serial.println("Box 2 set to RED");*/}
}

void setBoardColor(int board, int red, int blue, int green) //RGB
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

void CheckifChosenCorectly(int rnd,int var, int sequence[])
{
  if (rnd == sequence[var]) {MoveMe_Correct(); /*Serial.println("The correct one was moved");*/ delay(1000);}
  if (rnd != sequence[var]) {MoveMe_WRONG(); /*Serial.println("That was the incorrect movement");*/ delay(1000);}
}