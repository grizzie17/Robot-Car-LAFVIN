#ifndef H_CARDETAILS
#define H_CARDETAILS

#define CAR_MFG "LAFVIN"


//--- Pin Assignments ---------------------------
const uint8_t kPinMotorRightSpeed = 3;    // ENA gray (PWM)
const uint8_t kPinMotorRightFWD = 4;      // IN1 purple
const uint8_t kPinMotorRightREV = 5;      // IN2 blue
const uint8_t kPinMotorLeftFWD = 6;       // IN3 green
const uint8_t kPinMotorLeftREV = 7;       // IN4 yellow
const uint8_t kPinMotorLeftSpeed = 11;    // ENB orange (PWM)

const uint8_t kPinServo = 2;
const uint8_t kPinSonicEcho = 12;
const uint8_t kPinSonicTrig = 13;

const int k_nServoMaxSweep = 180 - 20;
const int k_nServoMinSweep = 0;


// range -30 to 30 (representing percentage) left is negative
const int kMotorAdjustment = -20;


#endif    // H_CARDETAILS
