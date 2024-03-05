/*
    Corrugator

    Cardboard based combat robot using an electric chainsaw for a weapon.

    Arduino Nano control board.
    TB6612FNG Dual H-Bridge carrier board for motor control.
    2x Inland (Microcenter) 2 channel relay boards for weapon power and weapon direction control.
    2.4 Ghz 6 channel RC controller - off brand, channel 2 doesn't work yet

    Joshua Brown 2024
*/

#include "box-bots-v1.h"

#define   forward 0
#define   reverse 1
#define   coast   2
#define   brake   3
#define   rightMotor  0
#define   leftMotor 1

#define   in_ch1  10    // input channel one is on Steering
#define   in_ch2  11    // input channel two is on Throttle
#define   in_ch3  12    // input channel three is Weapon SAW!
#define   maxWait 25000 // longest time in uS to wait for a radio pulse
#define   deadBand 20   // minimum departure from center before movement is registered

#define sawPowerPin 19
#define sawDirectionPin 18
#define relayOpen HIGH
#define relayClosed LOW

int ch1; // Steering - Joystick x-axis
int ch2; // Thottle - Joystick y-axis
int ch3; // Weapon Switch
int throttle = 0;
int spin = 0;
int rightMotorSpeed = 0;
int leftMotorSpeed = 0;
byte  oldDirection = 0; //for troubleshooting stuttering left motor problem
byte  newDirection = 0;


void motorFunction(byte function, byte motor) {
  switch (motor) {
    case leftMotor:
      switch (function) {
        case forward:
          digitalWrite(lpin1, HIGH);
          digitalWrite(lpin2, LOW);
          break;
        case reverse:
          digitalWrite(lpin1, LOW);
          digitalWrite(lpin2, HIGH);
          break;

        case brake:
          digitalWrite(lpin1, HIGH);
          digitalWrite(lpin2, HIGH);
          break;

        default:  // coast condition
          digitalWrite(lpin1, LOW);
          digitalWrite(lpin2, LOW);
          break;
      }
      break;
    case rightMotor:
      switch (function) {
        case forward:
          digitalWrite(rpin1, HIGH);
          digitalWrite(rpin2, LOW);
          break;

        case reverse:
          digitalWrite(rpin1, LOW);
          digitalWrite(rpin2, HIGH);
          break;

        case brake:
          digitalWrite(rpin1, HIGH);
          digitalWrite(rpin2, HIGH);
          break;

        default:  // coast condition
          digitalWrite(rpin1, LOW);
          digitalWrite(rpin2, LOW);
      }
      break;
    default:
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(lpwm, OUTPUT);
  pinMode(lpin1, OUTPUT);
  pinMode(lpin2, OUTPUT);
  pinMode(rpwm, OUTPUT);
  pinMode(rpin1, OUTPUT);
  pinMode(rpin2, OUTPUT);
  pinMode(standby, OUTPUT);

  pinMode(in_ch1, INPUT);       // channel one of RC receiver, x-axis steering
  pinMode(in_ch2, INPUT);       // channel two of RC receiver, y-axis throttle
  pinMode(in_ch3, INPUT);       // channel six of RC receiver, weapon
  pinMode(sawPowerPin, OUTPUT);
  pinMode(sawDirectionPin, OUTPUT);

  digitalWrite(lpin1, LOW);
  digitalWrite(lpin2, LOW);
  digitalWrite(rpin1, LOW);
  digitalWrite(rpin2, LOW);
  digitalWrite(standby, HIGH);

  digitalWrite(sawPowerPin, relayOpen);
  digitalWrite(sawDirectionPin, relayClosed);

 Serial.begin(9600);
}

void loop() { // about 31 Hz is the loop speed TODO, maybe different order of pulseIn would give better speeds?
  // pulsein returning value of 1000 to 2000 (1500 default neutral position)
  // All Numbers are with transmitter channels in Normal position

  ch1 = pulseIn(in_ch1, HIGH, maxWait); // Steering : 1000 Left, 2000 Right
  // delay(5); // the MN-6 rx needs a delay between these two pulseIn commands...
  ch2 = pulseIn(in_ch2, HIGH, maxWait); // Throttle : 1000 Reverse, 2000 Forward
  ch3 = pulseIn(in_ch3, HIGH, maxWait); // Weapon : 1000 Reverse, 2000 Forward, 1300 to 1700 OFF!


 Serial.print("ch1: ");
 Serial.print(ch1);
 Serial.print(", ch2: ");
 Serial.print(ch2);
 Serial.print(", ch3: ");
 Serial.println(ch3);

  // handle the case in which the signals time
  if (ch1 < 800) {
    ch1 = 1500;
  }
  if (ch2 < 800) {
    ch2 = 1500;
  }

  ch1 = map(ch1, 1000, 2000, -255, 255); //center over 500
  ch2 = map(ch2, 1000, 2000, -255, 255); //center over 500

  if (abs(ch1) < deadBand) {
    ch1 = 0;
  }
  if (abs(ch2) < deadBand) {
    ch2 = 0;
  }
  spin = -1 * ch1;
  throttle = -1 * ch2;

  rightMotorSpeed = constrain( throttle + spin, -255, 255);
  leftMotorSpeed = constrain( throttle - spin, -255, 255 );

  if (rightMotorSpeed < 0) {  // outside deadband, in reverse
    //       Serial.print(" Right Back ");
    motorFunction(reverse, rightMotor);
  }
  else {
    //       Serial.print(" Right Fwd ");
    motorFunction(forward, rightMotor);
  }
  if (leftMotorSpeed < 0) {
    //       Serial.print(" Left Back ");
    motorFunction(reverse, leftMotor);
    newDirection = reverse;
  }
  else {
    newDirection = forward;
    //       Serial.print(" Left Fwd ");
    motorFunction(forward, leftMotor);
    leftMotorSpeed = (int)(leftMotorSpeed);
  }
  if (oldDirection != newDirection) {
    //        Serial.print("@");
  }
  oldDirection = newDirection;

  analogWrite(lpwm, abs(leftMotorSpeed));
  analogWrite(rpwm, abs(rightMotorSpeed));

  if (ch3 > 1700) {
    digitalWrite(sawPowerPin, relayClosed);
    digitalWrite(sawDirectionPin, relayOpen);
  } else if (ch3 < 1300) {
    digitalWrite(sawPowerPin, relayClosed);
    digitalWrite(sawDirectionPin, relayClosed);
  } else {
    digitalWrite(sawPowerPin, relayOpen);
    digitalWrite(sawDirectionPin, relayOpen);
  }

  delay(5);  // helps with stability, I believe, will need to experiment
}
