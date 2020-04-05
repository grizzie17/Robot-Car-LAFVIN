
//===============================================
/// @file: RobotCar.ino
///
/// Arduino program to control a Robot RobotCar
/// using an Arduino UNO.
///
/// The car kit is supplied by LAFVIN
//===============================================

#include "cardetails.h"
#include "delay.h"
#include "sonic.h"
//#include <HCSR04.h>
#include <Servo.h>

#define CAR_VERSION CAR_MFG ": " __DATE__ " " __TIME__

unsigned long g_uTimeCurrent = 0;  ///< millis


//===============================================
//	CODE
//===============================================


//-----------------------------------------------
/// Sonic
//-----------------------------------------------

// HCSR04 g_tHCSR04( kPinHCSRTrig, kPinHCSREcho, 20, 4000 );
// UltraSonicDistanceSensor g_tSonicSensor( kPinSonicTrig, kPinSonicEcho );
CSonic g_tSonic;
CDelay g_tSonicDelay( 250 );
double g_dSonicDistance = 1000.0;

void
sonic_setup()
{
	g_dSonicDistance = 1000.0;
	g_tSonic.init( kPinSonicTrig, kPinSonicEcho );
}


void
sonic_loop()
{
	if ( g_tSonicDelay.timesUp( g_uTimeCurrent ) )
	{
		g_dSonicDistance = g_tSonic.getDistanceCm();
		if ( g_dSonicDistance < 0 )
			g_dSonicDistance = 1000;
		// g_dSonicDistance = g_tSonicSensor.measureDistanceCm();
		Serial.println( g_dSonicDistance );
		// Serial.println( g_tHCSR04.distanceInMillimeters() );
		// Serial.println( g_tHCSR04.ToString() );
	}
}

//-----------------------------------------------
/// Motor
//-----------------------------------------------
unsigned long g_uMotorDelay = 5000;
CDelay g_tMotorDelay( g_uMotorDelay );
int g_nMotorAdjustment = 0;  // range -30 to 30 (representing percentage)
enum { MOTOR_CW, MOTOR_STOP1, MOTOR_CCW, MOTOR_STOP2 } g_eMotorState;

void
motor_iwrite( int nSpeed, int ena, int inCW, int inCCW )
{
	enum { CCW, CW } eDirection;
	eDirection = CCW;
	if ( 0 < nSpeed )
	{
		eDirection = CCW;
		Serial.print( "CCW: " );
	}
	else
	{
		eDirection = CW;
		Serial.print( "CW:  " );
	}
	nSpeed = min( abs( nSpeed ), 100 );
	if ( 0 < nSpeed )
	{
		nSpeed = map( nSpeed, 0, 100, 64 + 16 + 8, 255 );
		Serial.print( "nSpeed=" );
		Serial.println( nSpeed );
		switch ( eDirection )
		{
		case CCW:
			// analogWrite( inCW, LOW );
			// analogWrite( inCCW, nSpeed );
			digitalWrite( inCW, LOW );
			digitalWrite( inCCW, HIGH );
			break;
		case CW:
		default:
			// analogWrite( inCW, nSpeed );
			// analogWrite( inCCW, LOW );
			digitalWrite( inCW, HIGH );
			digitalWrite( inCCW, LOW );
			break;
		}
		analogWrite( ena, nSpeed );
	}
	else
	{
		nSpeed = 0;
		analogWrite( ena, nSpeed );
		// analogWrite( inCW, LOW );
		// analogWrite( inCCW, LOW );
		digitalWrite( inCW, LOW );
		digitalWrite( inCCW, LOW );
	}
}

void
motor_write( int nSpeedLeft, int nSpeedRight )
{
	// calculate adjustment
	if ( 0 != g_nMotorAdjustment )
	{
		double a = abs( double( g_nMotorAdjustment ) / 100.0 );
		// Serial.print( "adjust=" );
		// Serial.println( a );
		if ( 0 < g_nMotorAdjustment )
		{
			nSpeedLeft -= abs( g_nMotorAdjustment );
			nSpeedRight += abs( g_nMotorAdjustment );
		}
		else
		{
			nSpeedLeft += abs( g_nMotorAdjustment );
			nSpeedRight -= abs( g_nMotorAdjustment );
		}
	}

	Serial.print( "Left=" );
	Serial.println( nSpeedLeft );
	motor_iwrite( nSpeedLeft, kPinMotorLeftSpeed, kPinMotorLeftCW, kPinMotorLeftCCW );
	Serial.print( "Right=" );
	Serial.println( nSpeedRight );
	motor_iwrite( nSpeedRight, kPinMotorRightSpeed, kPinMotorRightCW, kPinMotorRightCCW );
}

void
motor_rotateCW()
{
	motor_write( 50, 50 );
}
void
motor_rotateCCW()
{
	motor_write( -50, -50 );
}
void
motor_stop()
{
	analogWrite( kPinMotorLeftSpeed, 0 );
	analogWrite( kPinMotorRightSpeed, 0 );
	digitalWrite( kPinMotorRightCCW, LOW );
	digitalWrite( kPinMotorRightCW, LOW );
	digitalWrite( kPinMotorLeftCCW, LOW );
	digitalWrite( kPinMotorLeftCW, LOW );
}

void
motor_setup()
{
	g_eMotorState = MOTOR_CCW;
	g_tMotorDelay.init( g_uMotorDelay );
	pinMode( kPinMotorRightSpeed, OUTPUT );
	pinMode( kPinMotorLeftSpeed, OUTPUT );
	pinMode( kPinMotorRightCCW, OUTPUT );
	pinMode( kPinMotorRightCW, OUTPUT );
	pinMode( kPinMotorLeftCCW, OUTPUT );
	pinMode( kPinMotorLeftCW, OUTPUT );
}

void
motor_loop()
{
	if ( g_dSonicDistance < 20 )
	{
		switch ( g_eMotorState )
		{
		case MOTOR_STOP1:
		case MOTOR_STOP2:
			return;
			break;
		default:
			motor_stop();
			g_eMotorState = MOTOR_STOP1;
			g_uMotorDelay = 500;
			break;
		}
	}
	else
	{
		if ( g_tMotorDelay.timesUp( g_uTimeCurrent ) )
		{
			motor_write( 1, 1 );
			g_uMotorDelay = 1000;
			g_eMotorState = MOTOR_CCW;
		}
	}
}


//-----------------------------------------------
/// servo
//-----------------------------------------------
Servo g_tServo;  ///< servo control object
CDelay g_tServoDelay( 50 );
int g_nServoPos = 0;    ///< store servo position
int g_nServoState = 0;  ///< 0=ascend, 1=descend
// unsigned long g_uServoTimePrevious = 0;
const int k_nServoMaxSweep = 180 - 20;
const int k_nServoMinSweep = 0;

void
servo_setup()
{
	g_tServo.attach( kPinServo );
	g_nServoState = 0;
	g_nServoPos = k_nServoMinSweep;
	g_tServoDelay.init( 50 );
}

void
servo_middle()
{
	g_nServoPos = ( k_nServoMaxSweep - k_nServoMinSweep ) / 2;
	g_tServo.write( g_nServoPos );
	g_nServoState = 0;
}

void
servo_sweep()
{
	if ( g_tServoDelay.timesUp( g_uTimeCurrent ) )
	{
		g_tServo.write( g_nServoPos );
		if ( 0 == g_nServoState )
		{
			++g_nServoPos;
			if ( k_nServoMaxSweep <= g_nServoPos )
			{
				g_nServoPos = k_nServoMaxSweep;
				g_nServoState = 1;
			}
		}
		else
		{
			--g_nServoPos;
			if ( g_nServoPos < k_nServoMinSweep )
			{
				g_nServoPos = k_nServoMinSweep;
				g_nServoState = 0;
			}
		}
	}
}


//-----------------------------------------------
/// blink
//-----------------------------------------------
CDelay g_tBlinkDelay( 1000 );
// const unsigned long k_uBlinkInterval = 1000;
unsigned long g_uBlinkTimePrevious = 0;
int g_nBlinkLedState = LOW;

void
blink_setup()
{
	// set the builtin LED as output
	pinMode( LED_BUILTIN, OUTPUT );
	g_nBlinkLedState = LOW;
	g_uBlinkTimePrevious = 0;
}

void
blinker()
{
	if ( g_tBlinkDelay.timesUp( g_uTimeCurrent ) )
	{
		g_nBlinkLedState = LOW == g_nBlinkLedState ? HIGH : LOW;

		digitalWrite( LED_BUILTIN, g_nBlinkLedState );
	}
}

//===============================================
/// setup
//===============================================
void
setup()
{
	Serial.begin( 9600 );
	Serial.println( CAR_VERSION );
	// blink_setup();
	servo_setup();
	servo_middle();
	sonic_setup();
	motor_setup();
}


//===============================================
/// loop
//===============================================
void
loop()
{
	g_uTimeCurrent = millis();
	// blinker();
	// servo_sweep();
	sonic_loop();
	motor_loop();
}
