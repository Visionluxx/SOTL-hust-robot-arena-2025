// Code điều khiển hệ thống ESP8266 và Banhmi Viab với PS1 qua ESP-NOW và I2C
// Gồm: ESP8266 phát, ESP8266 nhận và Banhmi Viab, với đầy đủ chi tiết, cấu hình và chú thích

// ====================================
//           1. ESP8266 PHÁT
// ====================================
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PS2X_lib.h>

PS2X ps2x;  // Đối tượng điều khiển PS1
uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56}; // Địa chỉ MAC của ESP8266 nhận

struct DataPacket {
  byte joystickLeftY;
  byte joystickRightX;
  bool L1, R1, Green, Blue, Red, Pink;
};

DataPacket data;

void setup() {
  Serial.begin(115200);
  ps2x.config_gamepad(13, 12, 14, 15, true, true);  // config_gamepad(CMD, DAT, CLK, ATT)
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW khởi động thất bại");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  ps2x.read_gamepad(false, 0);
  data.joystickLeftY = ps2x.Analog(PSS_LY);
  data.joystickRightX = ps2x.Analog(PSS_RX);
  data.L1 = ps2x.Button(PSB_L1);
  data.R1 = ps2x.Button(PSB_R1);
  data.Green = ps2x.Button(PSB_GREEN);
  data.Blue = ps2x.Button(PSB_BLUE);
  data.Red = ps2x.Button(PSB_RED);
  data.Pink = ps2x.Button(PSB_PINK);
  esp_now_send(receiverAddress, (uint8_t *)&data, sizeof(data));
  delay(50);
}

// ====================================
//           2. ESP8266 NHẬN
// ====================================
#include <Wire.h>
#define BANHMI_ADDR 0x08
DataPacket receivedData;

void onReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Wire.beginTransmission(BANHMI_ADDR);
  Wire.write((uint8_t *)&receivedData, sizeof(receivedData));
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onReceive);
}

void loop() {}

// ====================================
//           3. BANHMI VIAB
// ====================================
#include <Wire.h>
#include <Servo.h>
Servo servo;

// Chân điều khiển động cơ 555
#define MOTOR1_PIN1 2
#define MOTOR1_PIN2 3
#define MOTOR2_PIN1 4
#define MOTOR2_PIN2 5

// Động cơ bước với A4988
#define STEPPER1_STEP 6
#define STEPPER1_DIR 7
#define STEPPER2_STEP 8
#define STEPPER2_DIR 9

void setup() {
  Serial.begin(115200);
  Wire.begin(0x08);
  Wire.onReceive(receiveEvent);
  servo.attach(10);
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  pinMode(STEPPER1_STEP, OUTPUT);
  pinMode(STEPPER1_DIR, OUTPUT);
  pinMode(STEPPER2_STEP, OUTPUT);
  pinMode(STEPPER2_DIR, OUTPUT);
}

void loop() {}

void receiveEvent(int howMany) {
  DataPacket receivedData;
  Wire.readBytes((char *)&receivedData, sizeof(receivedData));

  // Động cơ 555
  if (receivedData.joystickLeftY < 100) forward();
  else if (receivedData.joystickLeftY > 150) backward();

  if (receivedData.joystickRightX < 100) rotateLeft();
  else if (receivedData.joystickRightX > 150) rotateRight();

  // Động cơ bước
  if (receivedData.L1) stepper1Forward();
  if (receivedData.R1) stepper1Backward();
  if (receivedData.Green) stepper2Forward();
  if (receivedData.Blue) stepper2Backward();

  // Servo
  if (receivedData.Red) servo.write(90);
  if (receivedData.Pink) servo.write(-90);
}

void forward() {
  digitalWrite(MOTOR1_PIN1, HIGH);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, HIGH);
  digitalWrite(MOTOR2_PIN2, LOW);
}

void backward() {
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, HIGH);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, HIGH);
}

void rotateLeft() {
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, HIGH);
  digitalWrite(MOTOR2_PIN1, HIGH);
  digitalWrite(MOTOR2_PIN2, LOW);
}

void rotateRight() {
  digitalWrite(MOTOR1_PIN1, HIGH);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, HIGH);
}

void stepper1Forward() {
  digitalWrite(STEPPER1_DIR, HIGH);
  digitalWrite(STEPPER1_STEP, HIGH);
  delayMicroseconds(1000);
  digitalWrite(STEPPER1_STEP, LOW);
  delayMicroseconds(1000);
}

void stepper1Backward() {
  digitalWrite(STEPPER1_DIR, LOW);
  digitalWrite(STEPPER1_STEP, HIGH);
  delayMicroseconds(1000);
  digitalWrite(STEPPER1_STEP, LOW);
  delayMicroseconds(1000);
}

void stepper2Forward() {
  digitalWrite(STEPPER2_DIR, HIGH);
  digitalWrite(STEPPER2_STEP, HIGH);
  delayMicroseconds(1000);
  digitalWrite(STEPPER2_STEP, LOW);
  delayMicroseconds(1000);
}

void stepper2Backward() {
  digitalWrite(STEPPER2_DIR, LOW);
  digitalWrite(STEPPER2_STEP, HIGH);
  delayMicroseconds(1000);
  digitalWrite(STEPPER2_STEP, LOW);
  delayMicroseconds(1000);
}

// ====================================
//           KẾT THÚC
// ====================================
// Mã hoàn chỉnh cho hệ thống điều khiển với đầy đủ chú thích và các hàm điều khiển chi tiết.
