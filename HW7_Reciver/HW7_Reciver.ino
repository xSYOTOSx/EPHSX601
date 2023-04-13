// ESP Board MAC Address:  C8:C9:A3:5B:DA:BB
#include <ESP8266WiFi.h> 
#include <espnow.h>

typedef struct struct_message 
{
    int id;
    float x;
} struct_message;
// Create a struct_message called myData
struct_message myData;
// Create a structure to hold the readings from each board
struct_message board0; struct_message board1; 
// Create an array with all the structures
struct_message boardsStruct[15] = {board0, board1};

void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len)
{
  char macStr[18];
  Serial.print("Packet received from: "); 
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID: %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id].x = myData.x;
  Serial.print("x value: "); Serial.println(boardsStruct[myData.id].x);
  Serial.println();
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
  Serial.println("The following values are from the reciver");
}

void loop()
{
  
}
