#include <SoftwareSerial.h>

// ---------- Bluetooth Module ----------
// HC-05/HC-06: connect module TX -> Arduino pin 7, module RX -> Arduino pin 8
// (through a voltage divider if your module is 5V logic - HC-05 RX is usually 3.3V tolerant-sensitive)
#define BT_RX 7
#define BT_TX 8
SoftwareSerial bluetooth(BT_RX, BT_TX);

// ---------- IR Sensors ----------
#define LEFT_SENSOR   2
#define RIGHT_SENSOR  3

// ---------- Motor Driver (L298N) ----------
#define LEFT_MOTOR_FWD   5
#define LEFT_MOTOR_BWD   6
#define RIGHT_MOTOR_FWD  9
#define RIGHT_MOTOR_BWD  10

#define MOTOR_SPEED 150

// Mode flag: true = autonomous line-following, false = manual Bluetooth control
bool autonomousMode = true;

void setup() {
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  pinMode(LEFT_MOTOR_FWD, OUTPUT);
  pinMode(LEFT_MOTOR_BWD, OUTPUT);
  pinMode(RIGHT_MOTOR_FWD, OUTPUT);
  pinMode(RIGHT_MOTOR_BWD, OUTPUT);

  Serial.begin(9600);      // USB debug monitor
  bluetooth.begin(9600);   // HC-05/HC-06 default baud rate

  Serial.println("Robot ready. Send 'A' for Autonomous, 'M' for Manual mode.");
}

void loop() {
  // Check for incoming Bluetooth command
  if (bluetooth.available()) {
    char command = bluetooth.read();
    handleBluetoothCommand(command);
  }

  // Run the appropriate mode
  if (autonomousMode) {
    lineFollow();
  }
  // In manual mode, loop does nothing extra here -
  // the robot just holds whatever state the last command set (see handleBluetoothCommand)
}

// ---------- Bluetooth Command Handler ----------
void handleBluetoothCommand(char command) {
  switch (command) {
    case 'A': // switch to Autonomous / line-following mode
      autonomousMode = true;
      Serial.println("Mode: AUTONOMOUS");
      break;

    case 'M': // switch to Manual / Bluetooth control mode
      autonomousMode = false;
      stopMotors();
      Serial.println("Mode: MANUAL");
      break;

    // Manual driving commands - only acted on when NOT in autonomous mode
    case 'F':
      if (!autonomousMode) moveForward();
      break;
    case 'B':
      if (!autonomousMode) moveBackward();
      break;
    case 'L':
      if (!autonomousMode) turnLeft();
      break;
    case 'R':
      if (!autonomousMode) turnRight();
      break;
    case 'S':
      if (!autonomousMode) stopMotors();
      break;

    default:
      // ignore unrecognized characters (line endings, etc.)
      break;
  }
}

// ---------- Line Following Logic ----------
void lineFollow() {
  int leftValue  = digitalRead(LEFT_SENSOR);
  int rightValue = digitalRead(RIGHT_SENSOR);

  if (leftValue == HIGH && rightValue == HIGH) {
    moveForward();
  }
  else if (leftValue == LOW && rightValue == HIGH) {
    turnLeft();
  }
  else if (leftValue == HIGH && rightValue == LOW) {
    turnRight();
  }
  else if (leftValue == LOW && rightValue == LOW) {
    stopMotors();
  }
}

// ---------- Motor Control Functions ----------
void moveForward() {
  analogWrite(LEFT_MOTOR_FWD, MOTOR_SPEED);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  analogWrite(RIGHT_MOTOR_FWD, MOTOR_SPEED);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

void moveBackward() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  analogWrite(LEFT_MOTOR_BWD, MOTOR_SPEED);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  analogWrite(RIGHT_MOTOR_BWD, MOTOR_SPEED);
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  analogWrite(RIGHT_MOTOR_FWD, MOTOR_SPEED);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

void turnRight() {
  analogWrite(LEFT_MOTOR_FWD, MOTOR_SPEED);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

void stopMotors() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}
