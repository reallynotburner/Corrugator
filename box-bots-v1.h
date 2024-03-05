#define   lpwm    6     // pulse width modulation for left motor is pin 3
#define   lpin1   8    // left control pin one is pin 4
#define   lpin2   9     // left control pin two is pin 5
#define   standby 7     // standby pin is 6 - LOW=motor driver off, HIGH=motor driver on for new board
#define   rpin1   5     // right control pin one is pin 7
#define   rpin2   4     // right control pin two is pin 8
#define   rpwm    3     // pulse width modulation for right motor is pin 9 for new board

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

void boxBotSetup () {
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

  digitalWrite(lpin1, LOW);
  digitalWrite(lpin2, LOW);
  digitalWrite(rpin1, LOW);
  digitalWrite(rpin2, LOW);
  digitalWrite(standby, HIGH);
}

