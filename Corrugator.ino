/*
    Corrugator

    Cardboard based combat robot using an electric chainsaw for a weapon.

    Arduino Nano control board.
    TB6612FNG Dual H-Bridge carrier board for motor control.
    2x Inland (Microcenter) 2 channel relay boards for weapon power and weapon direction control.
    MN-6 rx 2.4 Ghz 6 channel RC receiver

    Joshua Brown 2024
*/

#include "box-bots-v1.h"

#define sawPowerPin 19
#define sawDirectionPin 18

#define forwards true
#define backwards false
#define relayOpen HIGH
#define relayClosed LOW
bool previousButtonState = false;
bool previousSawDirection = forwards;
bool previousSawPower = false;

void setup() {
  // do the usual setup for box bots
  boxBotSetup();

  // custom code to handle the weapon on Corrugator
  pinMode(sawPowerPin, OUTPUT);
  pinMode(sawDirectionPin, OUTPUT);
  digitalWrite(sawPowerPin, relayOpen);
  digitalWrite(sawDirectionPin, relayClosed);

  // a way to communicate data from the box-bot to the computer
  Serial.begin(9600);
}

// standard arduino loop, on box-bots runs about 30 times a second (Hz).
void loop() {
  // the MN-6 rx 2.4 Ghz 6 channel RC receiver needs a minimum 3ms delay after every pulse in.
  ch1 = pulseIn(in_ch1, HIGH, maxWait); // Steering : 1000 Left, 2000 Right
  delay(5);
  ch2 = pulseIn(in_ch2, HIGH, maxWait); // Throttle : 1000 Reverse, 2000 Forward
  delay(5);
  ch3 = pulseIn(in_ch3, HIGH, maxWait); // Weapon : 1000 Reverse, 2000 Forward, 1300 to 1700 OFF!
  delay(5);

  Serial.print("ch1: ");
  Serial.print(ch1);
  Serial.print(", ch2: ");
  Serial.print(ch2);
  Serial.print(", ch3: ");
  Serial.println(ch3);

  // handle the case in which the pulse widths are too brief to make sense
  if (ch1 < 800) {
    ch1 = 1500;
  }
  if (ch2 < 800) {
    ch2 = 1500;
  }

  if (ch3 < 800 || ch3 > 2200) {
    ch3 = 1500;
  }

  ch1 = map(ch1, 1000, 2000, -255, 255); // center over 1500
  ch2 = map(ch2, 1000, 2000, -255, 255); // center over 1500

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
    motorFunction(reverse, rightMotor);
  }
  else {
    motorFunction(forward, rightMotor);
  }
  if (leftMotorSpeed < 0) {
    motorFunction(reverse, leftMotor);
    newDirection = reverse;
  }
  else {
    newDirection = forward;
    motorFunction(forward, leftMotor);
    leftMotorSpeed = (int)(leftMotorSpeed);
  }
 
  oldDirection = newDirection;

  analogWrite(lpwm, abs(leftMotorSpeed));
  analogWrite(rpwm, abs(rightMotorSpeed));

  // handle the weapon signal
  // alternating between on and off,
  // and forward and backward
  bool currentButtonState = ch3 >= 1500;

  if (currentButtonState != previousButtonState) { // button state changed
    delay(100);  // let relay stabilize
    previousButtonState = currentButtonState;
    
    if (previousSawPower) { // turn it off
      previousSawPower = false;
      digitalWrite(sawPowerPin, relayOpen);
    } else { // procedure for changing direction
      digitalWrite(sawPowerPin, relayOpen);
      if (previousSawDirection) { // was going forward last time
        digitalWrite(sawDirectionPin, relayOpen); // so go backwards
      } else {
        digitalWrite(sawDirectionPin, relayClosed); // was backwards, so go forward
      }
      
      previousSawPower = true;
      previousSawDirection = !previousSawDirection; // store current value

      digitalWrite(sawPowerPin, relayClosed); // power it on
    }
  }
}

