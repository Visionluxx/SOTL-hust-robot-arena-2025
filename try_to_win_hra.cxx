#include <PS2X_lib.h>  // Thư viện PS2
#include <AccelStepper.h>  // Thư viện điều khiển động cơ bước
#include <Servo.h>  // Thư viện điều khiển servo

#define MOTOR_555_1_PIN1  32 //động cơ 555 số 1
#define MOTOR_555_1_PIN2  33
#define MOTOR_555_2_PIN1  25 //động cơ 555 số 2
#define MOTOR_555_2_PIN2  26

#define DIR_PIN1  12  // Động cơ bước 1
#define STEP_PIN1 14
#define DIR_PIN2  27  // Động cơ bước 2
#define STEP_PIN2  4

#define SERVO_PIN  5  // Servo

PS2X ps2x;  // Đối tượng PS2
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN1, DIR_PIN1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN2, DIR_PIN2);
Servo servo;

void setup() {
    Serial.begin(115200);
    servo.attach(SERVO_PIN);
    
    pinMode(MOTOR_555_1_PIN1, OUTPUT);
    pinMode(MOTOR_555_1_PIN2, OUTPUT);
    pinMode(MOTOR_555_2_PIN1, OUTPUT);
    pinMode(MOTOR_555_2_PIN2, OUTPUT);
    
    stepper1.setMaxSpeed(1000);
    stepper1.setAcceleration(500);
    stepper2.setMaxSpeed(1000);
    stepper2.setAcceleration(500);

    if (ps2x.config_gamepad(13, 15, 2, 0, true, true) == 0) {
        Serial.println("PS2 connected!");
    } else {
        Serial.println("PS2 connection failed!");
    }
}

void loop() {
    ps2x.read_gamepad(false, false);
    
    int lx = ps2x.Analog(PSS_LX) - 128;
    int ly = ps2x.Analog(PSS_LY) - 128;
    int rx = ps2x.Analog(PSS_RX) - 128;
    int ry = ps2x.Analog(PSS_RY) - 128;
    
    // Điều khiển động cơ 555 bằng joystick trái
    if (ly < -20) {  // Hướng lên
        digitalWrite(MOTOR_555_1_PIN1, HIGH);
        digitalWrite(MOTOR_555_1_PIN2, LOW);
        digitalWrite(MOTOR_555_2_PIN1, HIGH);
        digitalWrite(MOTOR_555_2_PIN2, LOW);
    } else if (ly > 20) {  // Hướng xuống
        digitalWrite(MOTOR_555_1_PIN1, LOW);
        digitalWrite(MOTOR_555_1_PIN2, HIGH);
        digitalWrite(MOTOR_555_2_PIN1, LOW);
        digitalWrite(MOTOR_555_2_PIN2, HIGH);
    } else {
        digitalWrite(MOTOR_555_1_PIN1, LOW);
        digitalWrite(MOTOR_555_1_PIN2, LOW);
        digitalWrite(MOTOR_555_2_PIN1, LOW);
        digitalWrite(MOTOR_555_2_PIN2, LOW);
    }
    
    // Điều khiển động cơ 555 theo joystick phải
    if (rx < -20) {  // Sang trái
        digitalWrite(MOTOR_555_1_PIN1, HIGH);
        digitalWrite(MOTOR_555_1_PIN2, LOW);
        digitalWrite(MOTOR_555_2_PIN1, LOW);
        digitalWrite(MOTOR_555_2_PIN2, HIGH);
    } else if (rx > 20) {  // Sang phải
        digitalWrite(MOTOR_555_1_PIN1, LOW);
        digitalWrite(MOTOR_555_1_PIN2, HIGH);
        digitalWrite(MOTOR_555_2_PIN1, HIGH);
        digitalWrite(MOTOR_555_2_PIN2, LOW);
    }
    
    // Điều khiển động cơ bước bằng nút bấm
    if (ps2x.Button(PSB_PAD_UP)) {
        stepper1.move(200);
    } else if (ps2x.Button(PSB_PAD_DOWN)) {
        stepper1.move(-200);
    }

    if (ps2x.Button(PSB_TRIANGLE)) {
        stepper2.move(200);
    } else if (ps2x.Button(PSB_CROSS)) {
        stepper2.move(-200);
    }
    
    stepper1.run();
    stepper2.run();
    
    // Điều khiển servo
    if (ps2x.Button(PSB_CIRCLE)) {
        servo.write(90);
    } else if (ps2x.Button(PSB_SQUARE)) {
        servo.write(0);
    }
}
