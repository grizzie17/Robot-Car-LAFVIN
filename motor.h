#ifndef H_MOTOR
#define H_MOTOR

#include "debug.h"

//===============================================
// class CMotor
//===============================================
class CMotor
{
public:
	CMotor( int ena, int in1, int in2, int in3, int in4, int enb );
	virtual ~CMotor();

public:
	/// tweak left to right speed
	void
	tweakSpeeds( int nAdjustment );

	/// values are in the range of -100 to 100
	/// where negative values cause motor to go
	/// in reverse
	void
	setSpeed(                  ///@return: nothing
			int nSpeedLeft,    ///< [in] speed of left motor
			int nSpeedRight    ///< [in] speed of right motor
	);

protected:
	void
	writePins( int nSpeed, int ena, int inREV, int inFWD );

	int m_pinRightSpeed;    //< ENA
	int m_pinRightFwd;      //< IN1
	int m_pinRightRev;      //< IN2
	int m_pinLeftFwd;       //< IN3
	int m_pinLeftRev;       //< IN4
	int m_pinLeftSpeed;     //< ENB
	int m_nTweak;
};

//===============================================
// CMotor implementation
//===============================================
// class lifecycle ------------------------------
CMotor::CMotor( int ena, int in1, int in2, int in3, int in4, int enb )
		: m_pinRightSpeed( ena )
		, m_pinRightFwd( in1 )
		, m_pinRightRev( in2 )
		, m_pinLeftFwd( in3 )
		, m_pinLeftRev( in4 )
		, m_pinLeftSpeed( enb )
		, m_nTweak( 0 )
{
	pinMode( m_pinRightSpeed, OUTPUT );
	pinMode( m_pinRightFwd, OUTPUT );
	pinMode( m_pinRightRev, OUTPUT );

	pinMode( m_pinLeftFwd, OUTPUT );
	pinMode( m_pinLeftRev, OUTPUT );
	pinMode( m_pinLeftSpeed, OUTPUT );
}

CMotor::~CMotor()
{}

// public functions -----------------------------
void
CMotor::tweakSpeeds( int nAdjustment )
{
	m_nTweak = nAdjustment;
}

void
CMotor::setSpeed( int nSpeedLeft, int nSpeedRight )
{
	// calculate adjustment
	if ( 0 != m_nTweak )
	{
		double a = abs( double( m_nTweak ) / 100.0 );
		// Serial.print( "adjust=" );
		// Serial.println( a );
		if ( 0 < m_nTweak )
		{
			nSpeedLeft += max( nSpeedLeft * a, 1.0 );
		}
		else
		{
			nSpeedRight += max( nSpeedRight * a, 1.0 );
		}
	}

	DEBUG_PRINT( "Left=" );
	DEBUG_PRINTLN( nSpeedLeft );
	writePins( nSpeedLeft, m_pinLeftSpeed, m_pinLeftFwd, m_pinLeftRev );
	DEBUG_PRINT( "Right=" );
	DEBUG_PRINTLN( nSpeedRight );
	writePins( nSpeedRight, m_pinRightSpeed, m_pinRightFwd, m_pinRightRev );
}

// protected functions --------------------------
void
CMotor::writePins( int nSpeed, int ena, int inFwd, int inRev )
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
			digitalWrite( inRev, LOW );
			digitalWrite( inFwd, HIGH );
			break;
		case REV:
		default:
			// analogWrite( inREV, nSpeed );
			// analogWrite( inFWD, LOW );
			digitalWrite( inRev, HIGH );
			digitalWrite( inFwd, LOW );
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
		digitalWrite( inRev, LOW );
		digitalWrite( inFwd, LOW );
		DEBUG_PRINTLN( "STOP" );
	}
}


#endif    // H_MOTOR
