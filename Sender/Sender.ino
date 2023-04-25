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
unsigned long timerDelay = 10; 
float Theshold = 0;
static int previousMovingState = -1; // for the if changed logic

float cal_x =0;
float cal_y =0;
float cal_z =0;
float cal_m =0;

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
  
  //This section setsup the ESP-NOW protocalls.
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


  // Sets up the MPU 6050
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
  
  //Various optiions for the MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);Serial.println("Accelerometer range set to: +-2G");
  //mpu.setAccelerometerRange(MPU6050_RANGE_4_G);Serial.print("Accelerometer range set to: +-4G");
  //mpu.setAccelerometerRange(MPU6050_RANGE_8_G);Serial.print("Accelerometer range set to: +-8G");
  //mpu.setAccelerometerRange(MPU6050_RANGE_16_G);Serial.print("Accelerometer range set to: +-16G");

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);Serial.println("Filter bandwidth set to: 5 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);Serial.print("Filter bandwidth set to: 10 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);Serial.print("Filter bandwidth set to: 21 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);Serial.print("Filter bandwidth set to: 44 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);Serial.print("Filter bandwidth set to: 94 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);Serial.print("Filter bandwidth set to: 184 Hz");
  //mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);Serial.print("Filter bandwidth set to: 260 Hz");

  // sets borad ID by mac adress
  if (WiFi.macAddress() == "C8:C9:A3:0B:46:21"){Serial.println("Board ID: 1");  myData.id = 1; }
  if (WiFi.macAddress() == "24:D7:EB:C6:E5:7A"){Serial.println("Board ID: 2");  myData.id = 2; }
  if (WiFi.macAddress() == "50:02:91:EA:E3:13"){Serial.println("Board ID: 3");  myData.id = 3; }
  if (WiFi.macAddress() == "8C:CE:4E:CE:E8:1A"){Serial.println("Board ID: 4");  myData.id = 4; }
  if (WiFi.macAddress() == "48:3F:DA:7D:FD:6C"){Serial.println("Board ID: 5");  myData.id = 5; }
  if (WiFi.macAddress() == "D8:BF:C0:C1:2C:41"){Serial.println("Board ID: 6");  myData.id = 6; }
  if (WiFi.macAddress() == "E0:98:06:8A:B6:B4"){Serial.println("Board ID: 7");  myData.id = 7; }
  if (WiFi.macAddress() == "BC:DD:C2:6C:7D:3A"){Serial.println("Board ID: 8");  myData.id = 8; }
  if (WiFi.macAddress() == "8C:CE:4E:CC:60:6D"){Serial.println("Board ID: 9");  myData.id = 9; }
  if (WiFi.macAddress() == "48:3F:DA:5F:A9:EE"){Serial.println("Board ID: 10"); myData.id = 10; }
  if (WiFi.macAddress() == "50:02:91:DA:F1:66"){Serial.println("Board ID: 11"); myData.id = 11; }
  if (WiFi.macAddress() == "EC:FA:BC:4C:36:65"){Serial.println("Board ID: 12"); myData.id = 12; }
  if (WiFi.macAddress() == "8C:AA:B5:69:B5:10"){Serial.println("Board ID: 13"); myData.id = 13; }
  if (WiFi.macAddress() == "98:F4:AB:BE:A0:AB"){Serial.println("Board ID: 14"); myData.id = 14; }

  //callibrates the mpu 6050 for its current orentation
  calibration();
}


void loop() 
{
  
  //if ((millis() - lastTime) > timerDelay) 
  //{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    myData.x = a.acceleration.x - (cal_x);
    myData.y = a.acceleration.y - (cal_y);
    myData.z = a.acceleration.z - (cal_z); 
    myData.m = sqrt( pow(myData.x,2)  + pow(myData.y,2)+ pow(myData.z,2) );
    
    if (myData.m > Theshold) { myData.isMoving = 1;}//True
    else                     { myData.isMoving = 0;}//False       
    //Printer(); //Serial Printer to see that the accerlation is doing
    
    if (myData.isMoving != previousMovingState) 
    { 
      //only sends to reciver if thiers a change to the moving
      esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
      previousMovingState = myData.isMoving; // update previous state
      //delay(500);
    }
    
    //esp_now_send(0, (uint8_t *) &myData, sizeof(myData)); // Use for testing, allows data to send every time
    lastTime = millis(); Serial.println(lastTime);
  //}
    
}


void Printer()
{
  /*************************************************************************
  This lets use the Serial print for debugguging purposes.  
  *************************************************************************/
  //Serial.println("    X    Y    Z   MAG");
  Serial.printf("ID:%d X:%f  Y:%f  Z:%f  MAG:%f  Is Moving? %d \n", 
                myData.id, myData.x, myData.y, myData.z,myData.m,myData.isMoving);
}


void calibration()
{
  Serial.println("Calibrating MPU6050...");

  // Accumulate sensor data for calibration
  float accel_x_sum = 0;
  float accel_y_sum = 0;
  float accel_z_sum = 0;
  float accel_m_sum = 0;
  const int numSamples = 1000; // Number of samples for calibration

  // Collect samples
  for (int i = 0; i < numSamples; i++) 
  {
    // Read raw sensor data
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Accumulate sensor data
    accel_x_sum = accel_x_sum + a.acceleration.x;
    accel_y_sum = accel_y_sum + a.acceleration.y;
    accel_z_sum = accel_z_sum + a.acceleration.z;
    //Serial.printf("get values? X:%f y:%f z:%f \n",a.acceleration.x,a.acceleration.y,a.acceleration.z);
    Serial.printf("Runnig SUM(%d)  x:%f y:%f z:%f \n",i,accel_x_sum,accel_y_sum,accel_z_sum );
    delay(10); 
  }

  // Calculate calibration offsets
  cal_x = accel_x_sum / numSamples;
  cal_y = accel_y_sum / numSamples;
  cal_z = accel_z_sum / numSamples;
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float T_x = a.acceleration.x - cal_x;
  float T_y = a.acceleration.y - cal_y;
  float T_z = a.acceleration.z - cal_z;
  cal_m = sqrt( pow(T_x,2)  + pow(T_y,2)+ pow(T_z,2) );
  Theshold = cal_m +3 ;

  Serial.println("--- Finished Calibration ---");
  Serial.printf("x:%f y:%f z:%f m:%f Theshold:%f \n", cal_x, cal_y, cal_z, cal_m, Theshold);
   
  
  delay(3000);
}


