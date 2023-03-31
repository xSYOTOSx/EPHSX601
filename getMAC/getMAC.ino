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
  
  if (WiFi.macAddress() == "C8:C9:A3:0B:46:21")
  { Serial.println("Hello, I am ESP32 No1"); }
  if (WiFi.macAddress() == "24:D7:EB:C6:E5:7A")
  { Serial.println("Hello, I am ESP32 No2"); }
  if (WiFi.macAddress() == "50:02:91:EA:E3:13")
  { Serial.println("Hello, I am ESP32 No3"); }
  if (WiFi.macAddress() == "8C:CE:4E:CE:E8:1A")
  { Serial.println("Hello, I am ESP32 No4"); }
}
 
void loop()
{

}