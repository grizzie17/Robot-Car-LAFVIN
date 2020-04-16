
#include "cardetails.h"
#include "delay.h"
#include "motor.h"

CDelay        m_tDelay;
CMotor        m_tMotor;
unsigned long g_uTimeCurrent;
unsigned long g_uCount = 0;

enum EState
{
	BEGIN,
	FWD_START,
	FWD_STOP,
	REV_START,
	REV_STOP,
	LEFT_START,
	LEFT_STOP,
	FINISH
} m_eState;


void
motor_begin()
{
	DEBUG_PRINTLN( "------------ begin ------------" );
	DEBUG_PRINTLN( g_uCount++ );
	m_eState = FWD_START;
}

void
motor_fwd_start()
{
	DEBUG_PRINTLN( "motor_fwd_start" );
	m_tDelay.newDelay( 5000 );
	m_tMotor.setSpeed( 50, 50 );
	m_eState = FWD_STOP;
}

void
motor_fwd_stop()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "motor_fwd_stop" );
		m_tMotor.stop();
		m_tDelay.newDelay( 3000 );
		m_eState = REV_START;
	}
}

void
motor_rev_start()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "motor_rev_start" );
		m_tMotor.setSpeed( -1, -1 );
		m_tDelay.newDelay( 5000 );
		m_eState = REV_STOP;
	}
}

void
motor_rev_stop()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "motor_rev_stop" );
		m_tMotor.stop();
		m_tDelay.newDelay( 3000 );
		m_eState = LEFT_START;
	}
}

void
motor_left_start()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "motor_left_start" );
		m_tMotor.setSpeed( -1, 1 );
		m_tDelay.newDelay( 5000 );
		m_eState = LEFT_STOP;
	}
}

void
motor_left_stop()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "motor_left_stop" );
		m_tMotor.stop();
		m_tDelay.newDelay( 3000 );
		m_eState = FINISH;
	}
}

void
motor_finish()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		DEBUG_PRINTLN( "motor_finish" );
		m_eState = BEGIN;
	}
}


void
setup()
{
	Serial.begin( 9600 );
	m_tMotor.setPins( kPinMotorRightSpeed, kPinMotorRightFWD, kPinMotorRightREV,
			kPinMotorLeftFWD, kPinMotorLeftREV, kPinMotorLeftSpeed );
	m_tDelay.init( 1000 );
	m_eState = BEGIN;
}

void
loop()
{
	g_uTimeCurrent = millis();
	switch ( m_eState )
	{
	case BEGIN:
		motor_begin();
		break;
	case FWD_START:
		motor_fwd_start();
		break;
	case FWD_STOP:
		motor_fwd_stop();
		break;
	case REV_START:
		motor_rev_start();
		break;
	case REV_STOP:
		motor_rev_stop();
		break;
	case LEFT_START:
		motor_left_start();
		break;
	case LEFT_STOP:
		motor_left_stop();
		break;

	case FINISH:
		motor_finish();
		break;
	default:
		m_eState = BEGIN;
		break;
	}
}
