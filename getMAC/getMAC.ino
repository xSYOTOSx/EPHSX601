#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  
  if (WiFi.macAddress() == "C8:C9:A3:0B:46:21") { Serial.println("Hello, I am ESP32 No1"); }
  if (WiFi.macAddress() == "24:D7:EB:C6:E5:7A") { Serial.println("Hello, I am ESP32 No2"); }
  if (WiFi.macAddress() == "50:02:91:EA:E3:13") { Serial.println("Hello, I am ESP32 No3"); }
  if (WiFi.macAddress() == "8C:CE:4E:CE:E8:1A") { Serial.println("Hello, I am ESP32 No4"); }
  if (WiFi.macAddress() == "48:3F:DA:7D:FD:6C") { Serial.println("Hello, I am ESP32 No5"); }
  if (WiFi.macAddress() == "D8:BF:C0:C1:2C:41") { Serial.println("Hello, I am ESP32 No6"); }
  if (WiFi.macAddress() == "E0:98:06:8A:B6:B4") { Serial.println("Hello, I am ESP32 No7"); }
  if (WiFi.macAddress() == "BC:DD:C2:6C:7D:3A") { Serial.println("Hello, I am ESP32 No8"); }
  if (WiFi.macAddress() == "8C:CE:4E:CC:60:6D") { Serial.println("Hello, I am ESP32 No9"); }
  if (WiFi.macAddress() == "48:3F:DA:5F:A9:EE") { Serial.println("Hello, I am ESP32 No10"); }
  if (WiFi.macAddress() == "50:02:91:DA:F1:66") { Serial.println("Hello, I am ESP32 No11"); }
  if (WiFi.macAddress() == "EC:FA:BC:4C:36:65") { Serial.println("Hello, I am ESP32 No12"); }
  if (WiFi.macAddress() == "8C:AA:B5:69:B5:10") { Serial.println("Hello, I am ESP32 No13"); }
  if (WiFi.macAddress() == "98:F4:AB:BE:A0:AB") { Serial.println("Hello, I am ESP32 No14"); }
  

}
 
void loop()
{

}