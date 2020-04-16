//===============================================
/// @file: RobotCar.ino
///
/// Arduino program to control a Robot RobotCar
/// using an Arduino UNO.
//===============================================

#include "cardetails.h"
#include "debug.h"
#include "delay.h"
#include "motor.h"
#include "sonar.h"
#include "sonic.h"

#define CAR_VERSION CAR_MFG ": " __DATE__ " " __TIME__

#ifndef TURN_SPEED
#	define TURN_SPEED 7
#endif
#ifndef TURN_TIME
#	define TURN_TIME 600
#endif
#ifndef REV_SPEED
#	define REV_SPEED 5
#endif
#ifndef REV_TIME
#	define REV_TIME 500
#endif


volatile unsigned long g_uTimeCurrent = 0;    // millis


//===============================================
//	CODE
//===============================================


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
		ST_START,
		ST_FWD_CHECK,
		ST_FWD_START,
		ST_FWD_MOVE,
		ST_REV_START,
		ST_REV_MOVE,
		ST_TURN_LEFT,
		ST_TURN_RIGHT,
		ST_TURN_STOP,
		ST_OBSTACLE,
		ST_FIND_DIRECTION
	};

	//	protected functions  --------------------

	void
	start();

	void
	fwdScan();

	/// is the way clear straight ahead
	void
	fwdCheck();

	void
	fwdStart();

	/// move straight ahead
	void
	fwdMove();

	void
	revStart();

	void
	revMove();

	void
	turnLeft();

	void
	turnRight();

	void
	turnStop();

	void
	avoidObstacle();

	void
	findNewDirection();

	//	protected data  -------------------------
	eState m_eState;
	eState m_eStateNext;
	int    m_nObstCount;
	bool   m_bActiveFwdScan;

	CDelay m_tDelay;
	CDelay m_tSonarPulseDelay;
	CDelay m_tSonarDelay;
	CSonar m_tSonar;
	CMotor m_tMotor;
};

// Class Lifecycle ==============================

car::car()
		: m_eState( ST_NONE )
		, m_eStateNext( ST_NONE )
		, m_nObstCount( 0 )
		, m_bActiveFwdScan( true )
		, m_tMotor()
		, m_tSonar()
		, m_tDelay()
		, m_tSonarDelay()
		, m_tSonarPulseDelay()
{}

car::~car()
{}

// Public Functions =============================

void
car::setup()
{
	// servo_setup();
	m_tDelay.init( 20 );
	m_tSonarDelay.init( 100 );
	m_tSonarPulseDelay.init( 20 );

	m_tSonar.init( kPinSonicTrig, kPinSonicEcho, kPinServo );
	m_tSonar.setSweepLimits( k_nServoMinSweep, k_nServoMaxSweep );
	m_tSonar.setMiniSweepOffset( 40 );
	m_tSonar.sonarFillLow();
	m_tMotor.setPins( kPinMotorRightSpeed, kPinMotorRightFWD, kPinMotorRightREV,
			kPinMotorLeftFWD, kPinMotorLeftREV, kPinMotorLeftSpeed );
	m_tMotor.tweakSpeeds( kMotorAdjustment );
	m_bActiveFwdScan = true;
	m_eState = ST_START;
}


void
car::loop()
{
	fwdScan();
	switch ( m_eState )
	{
	case ST_NONE:
	case ST_START:
		start();
		break;
	case ST_FWD_CHECK:
		fwdCheck();
		break;
	case ST_FWD_START:
		fwdStart();
		break;
	case ST_FWD_MOVE:
		fwdMove();
		break;
	case ST_REV_START:
		revStart();
		break;
	case ST_REV_MOVE:
		revMove();
		break;
	case ST_TURN_LEFT:
		turnLeft();
		break;
	case ST_TURN_RIGHT:
		turnRight();
		break;
	case ST_TURN_STOP:
		turnStop();
		break;
	case ST_OBSTACLE:
		avoidObstacle();
		break;
	case ST_FIND_DIRECTION:
		findNewDirection();
		break;
	default:
		break;
	}
}

// Protected Functions ==========================

void
car::fwdScan()
{
	if ( m_bActiveFwdScan )
	{
		if ( m_tSonarDelay.timesUp( g_uTimeCurrent ) )
		{
			m_tSonar.miniScan();
		}
		else if ( m_tSonarPulseDelay.timesUp( g_uTimeCurrent ) )
		{
			m_tSonar.pulseDistance();
		}
	}
}


void
car::start()
{
	m_bActiveFwdScan = true;
	m_tSonar.sonarFillHigh();
	m_eState = ST_FWD_CHECK;
}


void
car::fwdCheck()
{
	if ( m_tSonar.isMiniScanComplete() )
	{
		DEBUG_PRINTLN( "fwdCheck" );
		int nDist = m_tSonar.getMiniMinDistance();
		if ( 20 < nDist )
			m_eState = ST_FWD_START;
		else
			m_eState = ST_OBSTACLE;
	}
}

void
car::fwdStart()
{
	// if ( ! m_tSonar.isServoMiddle() )
	// {
	// 	m_tSonar.setServoMiddle();
	// 	delay( 100 );
	// 	return;
	// }
	DEBUG_PRINTLN( "fwdStart" );
	m_bActiveFwdScan = true;
	int dist = m_tSonar.getMiniMinDistance();
	if ( dist < 40 )
		m_tMotor.setSpeed( 1, 1 );
	else if ( dist < 80 )
		m_tMotor.setSpeed( 5, 5 );
	else if ( dist < 120 )
		m_tMotor.setSpeed( 10, 10 );
	else
		m_tMotor.setSpeed( 20, 20 );
	m_tDelay.newDelay( 20 );
	m_nObstCount = 0;
	m_eState = ST_FWD_MOVE;
}

void
car::fwdMove()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		// if ( ! m_tSonar.isServoMiddle() )
		// {
		// 	m_tSonar.setServoMiddle();
		// 	delay( 50 );
		// 	return;
		// }
		// DEBUG_PRINTLN( "fwdMove" );
		int dist = m_tSonar.getMiniMinDistance();
		if ( dist < 20 )
		{
			m_tMotor.stop();
			m_eState = ST_REV_START;
		}
		else if ( dist < 40 )
		{
			m_tMotor.setSpeed( 1, 1 );
		}
		else if ( dist < 80 )
		{
			m_tMotor.setSpeed( 5, 5 );
		}
		else if ( dist < 120 )
		{
			m_tMotor.setSpeed( 10, 10 );
		}
	}
}

void
car::revStart()
{
	DEBUG_PRINTLN( "revStart" );
	m_nObstCount = 0;
	m_bActiveFwdScan = false;
	m_tMotor.setSpeed( -REV_SPEED, -REV_SPEED );
	m_tDelay.newDelay( REV_TIME );
	m_eState = ST_REV_MOVE;
}

void
car::revMove()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "revMove" );
		m_tMotor.stop();
		m_eState = ST_OBSTACLE;
	}
}

void
car::turnLeft()
{
	DEBUG_PRINTLN( "turnLeft" );
	m_bActiveFwdScan = false;
	m_tDelay.newDelay( TURN_TIME );
	m_tMotor.setSpeed( -TURN_SPEED, TURN_SPEED );
	m_eState = ST_TURN_STOP;
}

void
car::turnRight()
{
	DEBUG_PRINTLN( "turnRight" );
	m_bActiveFwdScan = false;
	m_tDelay.newDelay( TURN_TIME );
	m_tMotor.setSpeed( TURN_SPEED, -TURN_SPEED );
	m_eState = ST_TURN_STOP;
}

void
car::turnStop()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "turnStop" );
		m_tMotor.stop();
		m_eState = ST_START;
	}
}

void
car::avoidObstacle()
{
	DEBUG_PRINTLN( "avoidObstacle" );
	m_tSonar.sonarFillLow();
	++m_nObstCount;
	if ( m_nObstCount < 2 )
	{
		m_bActiveFwdScan = false;
		m_eState = ST_FIND_DIRECTION;
	}
	else
	{
		m_eState = ST_REV_START;
	}
	m_tDelay.newDelay( 10 );
}

void
car::findNewDirection()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		if ( m_tSonar.isScanComplete() )
		{
			DEBUG_PRINTLN( "findNewDirection" );
			DEBUG_PRINTLN( "Scan Complete" );
			int nAngle = m_tSonar.getMaxDistanceAngle();
			if ( nAngle < 90 )
				m_eState = ST_TURN_RIGHT;
			else
				m_eState = ST_TURN_LEFT;
		}
		else
		{
			m_tSonar.scan();
		}
	}
}

car g_tCar;

//===============================================
/// setup
//===============================================
void
setup()
{
	Serial.begin( 9600 );
	DEBUG_PRINTLN( CAR_VERSION );
	// sonic_setup();
	// servo_setup();
	// sonar_middle();
	// radar_setLow();
	// motor_setup();
	g_tCar.setup();
}


//===============================================
/// loop
//===============================================
void
loop()
{
	g_uTimeCurrent = millis();
	// servo_sweep();
	// sonar_track();
	// motor_loop();
	g_tCar.loop();
}
