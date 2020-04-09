#ifndef H_CARDETAILS
#define H_CARDETAILS

#define CAR_MFG "LAFVIN"


//--- Pin Assignments ---------------------------
const int kPinMotorRightSpeed = 3;    // ENA gray (PWM)
const int kPinMotorRightFWD = 4;      // IN1 purple
const int kPinMotorRightREV = 5;      // IN2 blue
const int kPinMotorLeftFWD = 6;       // IN3 green
const int kPinMotorLeftREV = 7;       // IN4 yellow
const int kPinMotorLeftSpeed = 11;    // ENB orange (PWM)

const int kPinServo = 2;
const int kPinSonicEcho = 12;
const int kPinSonicTrig = 13;

// range -30 to 30 (representing percentage) left is negative
const int kMotorAdjustment = 15;


#endif    // H_CARDETAILS
