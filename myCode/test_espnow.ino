#include <PS2X_esp_lib.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

PS2X ps2x;

#define PS2_CLK 14 //D5
#define PS2_CMD 13 //D7
#define PS2_ATT 12 //D6 ATT or CS
#define PS2_DAT 02 //D4

uint8_t receiverMac[]={0x70,0xB8, 0xF6, 0xDB, 0xCA, 0x88};

typedef struct data{
  bool padUp;
  bool padDown;
  bool circle;
  bool square;
} data;

data myData;

 void OnSent (uint8_t *mac_addr, uint8_t sendStatus){
  Serial.print("gui");
  Serial.println(sendStatus==0 ? "succes":"fail");
 }

 void setup(){
  Serial.begin(115200);
  int error =ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init()!=0){
    Serial.println("Error when try to create espnow");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnSent);
  esp_now_add_peer(receiverMac, ESP_NOW_ROLE_SLAVE,1,NULL, 0);
  
 }

 void loop(){
  ps2x.read_gamepad(false, false);
  myData.padUp=ps2x.Button(PSB_PAD_UP);
  myData.padDown=ps2x.Button(PSB_PAD_DOWN);
  myData.circle=ps2x.Button(PSB_CIRCLE);
  myData.square=ps2x.Button(PSB_SQUARE);
  esp_now_send(receiverMac, (uint8_t *)&myData, sizeof (myData));
  delay(200);
 }
