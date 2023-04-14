//WIFI SetUp
#include <ESP8266WiFi.h>
#include <espnow.h>
int i = 0;
//Recivers MAC Address:  C8:C9:A3:5B:DD:7B
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x5B, 0xDD, 0x7B};
typedef struct struct_message 
{
    int id;
    int x;
} struct_message;
struct_message myData; //names the struct variable myData
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) 
{
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){ Serial.println("Delivery success"); }
  else{ Serial.println("Delivery fail"); }
}
 
void setup() 
{
  Serial.begin(115200);
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
  myData.id = 1;
}


void loop() 
{
  i++;
  myData.x = i; Serial.printf("x: %d \n", myData.x);
  esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  delay(1000);
}


