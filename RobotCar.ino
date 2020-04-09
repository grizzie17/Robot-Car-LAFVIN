//===============================================
/// @file: RobotCar.ino
///
/// Arduino program to control a Robot RobotCar
/// using an Arduino UNO.
///
/// The car kit is supplied by LAFVIN
//===============================================

#include "cardetails.h"
#include "debug.h"
#include "delay.h"
#include "motor.h"
#include "sonic.h"

#include <Servo.h>

#define CAR_VERSION CAR_MFG ": " __DATE__ " " __TIME__


unsigned long g_uTimeCurrent = 0;    ///< millis


const int k_nServoMaxSweep = 180 - 20;
const int k_nServoMinSweep = 0;
const int k_nServoMiddle = ( k_nServoMaxSweep - k_nServoMinSweep ) / 2;
const int k_nSonarMaxTrack = k_nServoMiddle + 10;
const int k_nSonarMinTrack = k_nServoMiddle - 10;

enum CarStates
{
	CAR_NULL,
	CAR_FWD,
	CAR_REV,
	CAR_OBSTACLE,
	CAR_TURN_LEFT,
	CAR_TURN_RIGHT
};


//===============================================
//	CODE
//===============================================


//-----------------------------------------------
/// Sonic
//-----------------------------------------------

// HCSR04 g_tHCSR04( kPinHCSRTrig, kPinHCSREcho, 20, 4000 );
// UltraSonicDistanceSensor g_tSonicSensor( kPinSonicTrig, kPinSonicEcho );
CSonic g_tSonic;
CDelay g_tSonicDelay( 100 );

void
sonic_setup()
{
	g_tSonic.init( kPinSonicTrig, kPinSonicEcho );
}


void
sonic_loop()
{
	if ( g_tSonicDelay.timesUp( g_uTimeCurrent ) )
	{
		double dDist = g_tSonic.getDistanceCm();
		DEBUG_PRINTLN( dDist );
	}
}

//-----------------------------------------------
/// Motor
//-----------------------------------------------
unsigned long g_uMotorDelay = 5000;
CDelay        g_tMotorDelay( g_uMotorDelay );
int           g_nMotorAdjustment = kMotorAdjustment;
double        g_dMotorDistance = 15.0;

enum
{
	MOTOR_REV,
	MOTOR_STOP1,
	MOTOR_FWD,
	MOTOR_STOP2
} g_eMotorState;

void
motor_calculateDistance( int nSpeedLeft, int nSpeedRight )
{
	int nSpeedAvg = ( nSpeedLeft + nSpeedRight ) / 2;
	g_dMotorDistance = map( nSpeedAvg, 1, 100, 10, 40 );
	DEBUG_PRINT( "Distance=" );
	DEBUG_PRINTLN( g_dMotorDistance );
}

void
motor_iwrite( int nSpeed, int ena, int inREV, int inFWD )
{
	enum
	{
		FWD,
		REV
	} eDirection;
	eDirection = FWD;
	if ( 0 < nSpeed )
	{
		eDirection = FWD;
		DEBUG_PRINT( "FWD: " );
	}
	else
	{
		eDirection = REV;
		DEBUG_PRINT( "REV:  " );
	}
	nSpeed = min( abs( nSpeed ), 100 );
	if ( 0 < nSpeed )
	{
		nSpeed = map( nSpeed, 0, 100, 64 + 16 + 8, 255 );
		DEBUG_PRINT( "nSpeed=" );
		DEBUG_PRINTLN( nSpeed );
		switch ( eDirection )
		{
		case FWD:
			// analogWrite( inREV, LOW );
			// analogWrite( inFWD, nSpeed );
			digitalWrite( inREV, LOW );
			digitalWrite( inFWD, HIGH );
			break;
		case REV:
		default:
			// analogWrite( inREV, nSpeed );
			// analogWrite( inFWD, LOW );
			digitalWrite( inREV, HIGH );
			digitalWrite( inFWD, LOW );
			break;
		}
		analogWrite( ena, nSpeed );
	}
	else
	{
		nSpeed = 0;
		analogWrite( ena, nSpeed );
		// analogWrite( inREV, LOW );
		// analogWrite( inFWD, LOW );
		digitalWrite( inREV, LOW );
		digitalWrite( inFWD, LOW );
		DEBUG_PRINTLN( "STOP" );
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
			nSpeedLeft += max( nSpeedLeft * a, 1.0 );
		}
		else
		{
			nSpeedRight += max( nSpeedRight * a, 1.0 );
		}
	}

	Serial.print( "Left=" );
	DEBUG_PRINTLN( nSpeedLeft );
	motor_iwrite( nSpeedLeft, kPinMotorLeftSpeed, kPinMotorLeftREV,
			kPinMotorLeftFWD );
	Serial.print( "Right=" );
	DEBUG_PRINTLN( nSpeedRight );
	motor_iwrite( nSpeedRight, kPinMotorRightSpeed, kPinMotorRightREV,
			kPinMotorRightFWD );
}

void
motor_rotateREV()
{
	motor_write( 50, 50 );
}
void
motor_rotateFWD()
{
	motor_write( -50, -50 );
}

void
motor_stop()
{
	analogWrite( kPinMotorLeftSpeed, 0 );
	analogWrite( kPinMotorRightSpeed, 0 );
	digitalWrite( kPinMotorRightFWD, LOW );
	digitalWrite( kPinMotorRightREV, LOW );
	digitalWrite( kPinMotorLeftFWD, LOW );
	digitalWrite( kPinMotorLeftREV, LOW );
	DEBUG_PRINTLN( "STOP" );
}

void
motor_setup()
{
	g_eMotorState = MOTOR_FWD;
	g_tMotorDelay.init( g_uMotorDelay );
	pinMode( kPinMotorRightSpeed, OUTPUT );
	pinMode( kPinMotorLeftSpeed, OUTPUT );
	pinMode( kPinMotorRightFWD, OUTPUT );
	pinMode( kPinMotorRightREV, OUTPUT );
	pinMode( kPinMotorLeftFWD, OUTPUT );
	pinMode( kPinMotorLeftREV, OUTPUT );
}

void
motor_loop()
{
	if ( sonar_minDistance( k_nSonarMinTrack, k_nSonarMaxTrack )
			< g_dMotorDistance )
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
			g_tMotorDelay.reset();
			break;
		}
	}
	else
	{
		if ( g_tMotorDelay.timesUp( g_uTimeCurrent ) )
		{
			motor_write( 25, 25 );
			motor_calculateDistance( 25, 25 );
			g_eMotorState = MOTOR_FWD;
		}
	}
}


//-----------------------------------------------
/// servo
//-----------------------------------------------
Servo  g_tServo;    ///< servo control object
CDelay g_tServoDelay( 10 );
int    g_nServoPos = 0;    ///< store servo position
int    g_nServoLow = 5000;
int    g_nServoHigh = -1;
int    g_nServoState = 0;    ///< 0=ascend, 1=descend

int g_aRadar[k_nServoMaxSweep + 1];

void
servo_setup()
{
	g_tServo.attach( kPinServo );
	g_tServoDelay.init( 10 );
}

void
radar_setHigh()
{
	g_nServoHigh = -1;
	g_nServoLow = 5000;
	for ( int i = k_nServoMinSweep; i < k_nServoMaxSweep; ++i )
		g_aRadar[i] = 5000;
}

void
radar_setLow()
{
	g_nServoHigh = -1;
	g_nServoLow = 5000;
	for ( int i = k_nServoMinSweep; i < k_nServoMaxSweep; ++i )
		g_aRadar[i] = -1;
}

void
sonar_middle()
{
	g_nServoPos = k_nServoMiddle;
	g_tServo.write( g_nServoPos );
	g_nServoState = 0;
}

void
sonar_scan( int nMin, int nMax )
{
	if ( g_tServoDelay.timesUp( g_uTimeCurrent ) )
	{
		if ( g_nServoPos < nMin || nMax < g_nServoPos )
		{
			g_nServoHigh = -1;
			g_nServoLow = 5000;
			if ( g_nServoPos < nMin )
			{
				g_nServoPos = nMin;
				g_nServoState = 0;    // ascend
			}
			else
			{
				g_nServoPos = nMax;
				g_nServoState = 1;
			}
			g_tServo.write( g_nServoPos );
			g_tServoDelay.reset();
			return;
		}
		double dist = g_tSonic.getDistanceCm();
		g_aRadar[g_nServoPos] = dist;
		g_tServo.write( g_nServoPos );
		if ( 0 == g_nServoState )
		{
			++g_nServoPos;
			if ( nMax <= g_nServoPos )
			{
				g_nServoPos = nMax;
				g_nServoHigh = nMax;
				g_nServoState = 1;
			}
		}
		else
		{
			--g_nServoPos;
			if ( g_nServoPos < nMin )
			{
				g_nServoPos = nMin;
				g_nServoLow = nMin;
				g_nServoState = 0;
			}
		}

		// DEBUG_PRINT( dist );
		// DEBUG_PRINT( " " );
		// DEBUG_PRINTLN( g_nServoPos );
	}
}

int
sonar_maxDistance( int nMin, int nMax )
{
	int nValue = -1;
	for ( int i = nMin; i <= nMax; ++i )
	{
		if ( nValue < g_aRadar[i] )
			nValue = g_aRadar[i];
	}
	return nValue;
}

int
sonar_minDistance( int nMin, int nMax )
{
	int nValue = 5000;
	for ( int i = nMin; i < nMax; ++i )
	{
		if ( g_aRadar[i] < nValue )
			nValue = g_aRadar[i];
	}
	// DEBUG_PRINT( "min dist=" );
	// DEBUG_PRINTLN( nValue );
	return nValue;
}

int
sonar_maxDistanceAngle( int nMin, int nMax )
{
	int nDist = -1;
	int nAngle = k_nServoMiddle;
	for ( int i = nMin; i < nMax; ++i )
	{
		if ( nDist < g_aRadar[i] )
		{
			nDist = g_aRadar[i];
			nAngle = i;
		}
	}
	return nAngle;
}

#if 0
void
sonar_sweep()
{
	sonar_scan( k_nServoMinSweep, k_nServoMaxSweep );
	if ( k_nServoMinSweep == g_nServoLow
			&& k_nServoMaxSweep - 1 == g_nServoHigh )
	{
		int nDirection
				= sonar_maxDistanceAngle( k_nServoMinSweep, k_nServoMaxSweep );
		if ( nDirection < k_nServoMiddle )
		{
			g_eCarState = CAR_TURN_RIGHT;
		}
		else
		{
			g_eCarState = CAR_TURN_LEFT;
		}
	}
}
#endif

void
sonar_track()
{
	sonar_scan( k_nSonarMinTrack, k_nSonarMaxTrack );
}


//-----------------------------------------------
// blink
//-----------------------------------------------
CDelay        g_tBlinkDelay( 1000 );
unsigned long g_uBlinkTimePrevious = 0;
int           g_nBlinkLedState = LOW;

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
// car
//===============================================
class car
{
	//	class lifecycle  ------------------------
public:
	car();
	virtual ~car();

public:
	//	public types  ---------------------------

	//	public functions  -----------------------
	void
	setup();
	void
	loop();

protected:
	//	protected types  ------------------------
	enum eState
	{
		ST_NONE,
		ST_FWD,
		ST_REV,
		ST_TURN_LEFT,
		ST_TURN_RIGHT,
		ST_OBSTACLE
	};

	//	protected functions  --------------------

	/// is the way clear straight ahead
	void
	checkAhead();

	/// move straight ahead
	void
	moveForward();

	void
	moveReverse();

	void
	turnLeft();

	void
	turnRight();

	//	protected data  -------------------------
	eState m_eState;
	eState m_eStateNext;
	CSonic m_tSonic;
	CMotor m_tMotor;
};

// Class Lifecycle ==============================

car::car()
		: m_eState( ST_NONE )
		, m_eStateNext( ST_NONE )
		, m_tSonic()
		, m_tMotor( kPinMotorLeftSpeed, kPinMotorLeftFWD, kPinMotorLeftREV,
				  kPinMotorRightFWD, kPinMotorRightREV, kPinMotorRightSpeed )
{
	m_tSonic.init( kPinSonicTrig, kPinSonicEcho );
}

car::~car()
{}

// Public Functions =============================

void
car::setup()
{
	servo_setup();
}


void
car::loop()
{
	g_uTimeCurrent = millis();
	switch ( m_eState )
	{
	case ST_NONE:
	case ST_FWD:
	case ST_REV:
	case ST_TURN_LEFT:
	case ST_TURN_RIGHT:
	case ST_OBSTACLE:
	default:
		break;
	}
}

// Protected Functions ==========================

void
car::checkAhead()
{}

void
car::moveForward()
{}

void
car::moveReverse()
{}

void
car::turnLeft()
{}

void
car::turnRight()
{}

car g_tCar;

//===============================================
/// setup
//===============================================
CarStates g_eCarState = CAR_NULL;
void
setup()
{
	Serial.begin( 9600 );
	DEBUG_PRINTLN( CAR_VERSION );
	// blink_setup();
	sonic_setup();
	servo_setup();
	sonar_middle();
	radar_setLow();
	motor_setup();
	g_tCar.setup();
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
	// sonar_track();
	motor_loop();
	g_tCar.loop();
}
