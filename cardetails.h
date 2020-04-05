#ifndef H_CARDETAILS
#define H_CARDETAILS

#define CAR_MFG "LAFVIN"


//--- Pin Assignments ---------------------------
const int kPinMotorRightSpeed = 3;  // ENA gray (PWM)
const int kPinMotorRightCCW = 4;    // IN1 purple
const int kPinMotorRightCW = 5;     // IN2 blue
const int kPinMotorLeftCCW = 6;     // IN3 green
const int kPinMotorLeftCW = 7;      // IN4 yellow
const int kPinMotorLeftSpeed = 11;  // ENB orange (PWM)

const int kPinServo = 2;
const int kPinSonicEcho = 12;
const int kPinSonicTrig = 13;


#endif  // H_CARDETAILS
