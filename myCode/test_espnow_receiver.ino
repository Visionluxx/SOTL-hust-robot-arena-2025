#include <ESP32Servo.h>
#include <WiFi.h>
#include<esp_now.h>

Servo myservo;

const int servoPin=4;

typedef struct data{
  bool padUp;
  bool padDown;
  bool circle;
  bool square;
} data;

data myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  if (myData.padUp){Serial.println("pad up"); myservo.write(90);}
  if (myData.padDown){Serial.println("pad down"); myservo.write(180);}
  if (myData.circle){Serial.println("circle"); myservo.write(45);}
  if (myData.square){Serial.println("square"); myservo.write(0);}
}

void setup(){
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init()!= ESP_OK){
    Serial.println("error");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
}
