/*********************************************************************************************
  This code is heavilly dependent on Rui Santos code on randomnerdtutorials.com. For complete 
  project details visit https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/.
  
  From here orginial code: "Permission is hereby granted, free of charge, to any person 
  obtaining a copy of this software and associated documentation files. The above copyright
  notice and this permission notice shall be included in all copies or substantial portions 
  of the Software."

  Authers: Ryan Coppens & Thresa Kelly

  Code To Do:
  - Voltage Monitering & Notification
  - failed data packet LED
  - Sucessful data packet LED
  *********************************************************************************************/
/*************************************************************************
This section is setting up some variables that control diffrent aspects of
the code. I put it here so it would be easy to update during itterations
when testing and calibration. 
- lastTime & timerDelay setup the refresh rates for sending out packets. 
- Threshold is the isMoving theshold. This is set the sensitivity on how 
  quickly one would need to push or pull the sensor for it to read. 
- cal_x, cal_y, cal_z are calibration parameters. every MPU 6050 is a 
  little bit diffrent and each need to be calibrated. Will explain further
  in a later section
*************************************************************************/
unsigned long lastTime = 0;
unsigned long timerDelay = 50; 
float Theshold = 12;

float cal_x;
float cal_y;
float cal_z;

//WIFI SetUp
#include <ESP8266WiFi.h>
#include <espnow.h>

//Accelermoiter SetUp
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu;



/*************************************************************************
This section setus up various things.
- Sets the broadcast adress as units MAC adress
- Recivers MAC Address:  C8:C9:A3:5B:DA:BB
- Setup the message struct. a note more for myself than any future readers
  is, a struct is like a class or an array that allows multiple data types
  (int, string, bool etc.). Acess variables in a structure (also known as 
  a member) use the . sytnex. For example myData.x . The struct layout 
  must math the recivers. 
- The onDataSent function allows us to serialy print if a message was 
  delivered sucessfuly or not. Realistlcly this is for debugging purposes
  only. 
*************************************************************************/
//Recivers MAC Address:  C8:C9:A3:5B:DA:BB
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x5B, 0xDA, 0xBB};
typedef struct struct_message 
{
    int id;
    float x;
    float y;
    float z;
    float m;
    bool isMoving;
} struct_message;
struct_message myData; //names the struct variable myData
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) 
{
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0)
    { Serial.println("Delivery success"); }
  else
    { Serial.println("Delivery fail"); }
}
 
void setup() 
{
  Serial.begin(115200);
  /*************************************************************************
  This section setsup the ESP-NOW protocalls.
  1) Firstly, theirs a loop that catches the program if it cant initilize
  2) It then sets the role as controller aka the sender in this many-to-one
     protocoll. 
  3) After successfully initializing ESP-NOW,  we register the callback 
     function that will be called when a message is sent. In this case, 
     register for the OnDataSent() function created previously.
  4) To send data to the receiver, you need to pair it as a peer. 
     The esp_now_add_peer() does this. the function needs the following 
     arguments in order: mac address, peer role, wi-fi channel, key, & 
     key length
  *************************************************************************/
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  /*************************************************************************
  This section setsup the Adafruits MPU6050 libery settings.
  1) Firstly, theirs a loop that catches the program if it cant initilize
  2) next theirs settings for what range we need to read for the MPU6050. at
     the time of this writing. The smaller the range the more sensitive the
     MPU 6050 is, so i selected the 2G range. 
  3) At htis time we are not using the gyro feture of the MPU6050
  4) This libary also has a low pass filter
  *************************************************************************/
  
  Serial.println("Adafruit MPU6050 test!");
  if (!mpu.begin()) 
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1) 
    { 
      delay(10); 
    }
  }
  Serial.println("MPU6050 Found!");
  

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);Serial.print("Accelerometer range set to: +-2G");
  //mpu.setAccelerometerRange(MPU6050_RANGE_4_G);Serial.print("Accelerometer range set to: +-4G");
  //mpu.setAccelerometerRange(MPU6050_RANGE_8_G);Serial.print("Accelerometer range set to: +-8G");
  //mpu.setAccelerometerRange(MPU6050_RANGE_16_G);Serial.print("Accelerometer range set to: +-16G");

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);Serial.print("Filter bandwidth set to: 5 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);Serial.print("Filter bandwidth set to: 10 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);Serial.print("Filter bandwidth set to: 21 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);Serial.print("Filter bandwidth set to: 44 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);Serial.print("Filter bandwidth set to: 94 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);Serial.print("Filter bandwidth set to: 184 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);Serial.print("Filter bandwidth set to: 260 Hz");

  


  /***************************************************************************
  This section allows us to set the predetermeded calibration settings for
  each sensor baised on the mac adress. This way we only need one one program
  for all the boards. This is also where we assign the board ID baised on the
  mac afress and add it to the myData struct. This is easier than trying to
  send and compare the mac adress on the reciver end. The calibration is done
  by running the program and making edits to the avrage to until it reads 
  'zero'. For enxample if while the chip is flat and not moving it reads an 
  avrage of 10.2 m/s^2 on the z axis, the a correction of -0.4 is added so it
  reads 9.8 m/s^2 ie zero for a non aceleratining object. 
  ID  1:C8:C9:A3:0B:46:21   ID  2:24:D7:EB:C6:E5:7A   ID  3:50:02:91:EA:E3:13
  ID  4:8C:CE:4E:CE:E8:1A   ID  5:    ID  6: 
  ID  7:    ID  8:    ID  9: 
  ID 10:    ID 11:    ID 12: 
  ID 13:    ID 14:    ID 15: 
  ID 16: 40:F5:20:26:B9:FB TEST Sender
  ID 17: 40:F5:20:28:A1:D4 TEST Reciver  
  ***************************************************************************/
  if (WiFi.macAddress() == "C8:C9:A3:0B:46:21")
  {
    Serial.println("Board ID: 1");  
    myData.id = 1;
    cal_x=0.15;
    cal_y=0.08;
    cal_z=-0.42;
    delay(5000);
  }
  if (WiFi.macAddress() == "24:D7:EB:C6:E5:7A")
  {
    Serial.println("Board ID: 2");  
    myData.id = 2;
    cal_x=0.15;
    cal_y=0.45;
    cal_z=-0.52;
    delay(5000);
  }
  if (WiFi.macAddress() == "50:02:91:EA:E3:13")
  {
    Serial.println("Board ID: 3");  
    myData.id = 3;
    cal_x=0;
    cal_y=0;
    cal_z=0;
    delay(5000);
  }
  if (WiFi.macAddress() == "8C:CE:4E:CE:E8:1A")
  {
    Serial.println("Board ID: 4");  
    myData.id = 4;
    cal_x=0;
    cal_y=0;
    cal_z=0;
    delay(5000);
  }

  if (WiFi.macAddress() == "40:F5:20:26:B9:FB")
  {
    Serial.println("Board ID: 4");  
    myData.id = 4;
    cal_x=0;
    cal_y=0;
    cal_z=0;
    delay(5000);
  }
}

  /*************************************************************************
  This is the main loop. It loops baised on the time delay that we set in 
  the beginnining. This frist gets the events from the MPU6050 and saves it
  the myData struct. myData.m is the magnitude of the acceleration, and is
  whats used to detect motion. The idea is that nomatter the end orentation
  we can pass a postive value for our comparititer. This magnitude is than
  compared to our theshold and the boolean value is then published
  *************************************************************************/
void loop() 
{
  
  if ((millis() - lastTime) > timerDelay) 
  {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    myData.x = a.acceleration.x + cal_x;
    myData.y = a.acceleration.y + cal_y;
    myData.z = a.acceleration.z + cal_z; 
    myData.m = sqrt( pow(myData.x,2)  + pow(myData.y,2)+ pow(myData.z,2) );
    if (myData.m > Theshold)
      { myData.isMoving = 1;}//True
    else
      { myData.isMoving = 0;}//False       
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
    
    
    //Plotter();
    Printer();
    //delay(500);


    lastTime = millis();
  }
}

void Plotter()
{
  /*************************************************************************
  This lets us use the arduino Serial plotter, which is a continious time v. 
  magnitude graph. This way we can see the SS errors. 
  *************************************************************************/
  Serial.print(myData.x); Serial.print(",");
  Serial.print(myData.y); Serial.print(",");
  Serial.print(myData.z); Serial.print(", ");
  Serial.print(myData.m); Serial.println("");  
}

void Printer()
{
  /*************************************************************************
  This lets use the Serial print for debugguging purposes.  
  *************************************************************************/
  Serial.println("    X    Y    Z   MAG");
  Serial.print(" ");
  Serial.print(myData.x);
  Serial.print(" ");
  Serial.print(myData.y);
  Serial.print(" ");
  Serial.print(myData.z);
  Serial.print(" ");
  Serial.print(myData.m);
  Serial.println("   Acceleration (m/s^2)");
  Serial.print("Is Moving?: "); Serial.println(myData.isMoving);
}
