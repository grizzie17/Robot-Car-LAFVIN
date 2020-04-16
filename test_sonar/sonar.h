#ifndef H_SONAR
#define H_SONAR

#include "debug.h"
#include "sonic.h"
#include <Servo.h>


#define FULLSWEEP -1


/// class to abstract the combination of
/// a servo with a sonic sensor
class CSonar
{
public:
	CSonar();
	CSonar( int nSonicTrig, int nSonicEcho, int nServoPin );
	virtual ~CSonar();

public:
	void
	init( int nSonicTrig, int nSonicEcho, int nServoPin );

	void
	setSweepLimits( int nMin, int nMax );

	void
	setMiniSweepOffset( int nOffset );

	void
	setServoPosition( int nPos );

	void
	setServoMiddle();

	bool
	isServoMiddle();

	void
	scan( int nMin = -1, int nMax = -1 );

	void
	miniScan( int nMin = -1, int nMax = -1 );

	bool
	isScanComplete( int nMin = -1, int nMax = -1 );

	bool
	isMiniScanComplete( int nMin = -1, int nMax = -1 );

	void
	sonarFillLow();

	void
	sonarFillHigh();

	int
	getServoPosition();

	double
	getDistanceCm();

	int
	getAngleMid();

	uint16_t
	getDistanceAtAngle( int n );

	uint16_t
	getMaxDistance( int nMin = -1, int nMax = -1 );

	uint16_t
	getMinDistance( int nMin = -1, int nMax = -1 );

	uint16_t
	getMiniMaxDistance( int nMin = -1, int nMax = -1 );

	uint16_t
	getMiniMinDistance( int nMin = -1, int nMax = -1 );

	uint16_t
	getMaxDistanceAngle( int nMin = -1, int nMax = -1 );


	void
	debugDumpSweep();

protected:
	CSonic m_tSonic;
	Servo  m_tServo;
	int    m_nServoPos;
	int    m_nServoMin;
	int    m_nServoMax;
	int    m_nServoMid;
	int    m_nServoMiniMin;
	int    m_nServoMiniMax;
	enum SweepState
	{
		ASCEND,
		DESCEND
	} m_eSweepState;
	int      m_nSweepLow;
	int      m_nSweepHigh;
	uint16_t m_aSweep[180 + 1];
};


CSonar::CSonar()
		: m_tSonic()
		, m_tServo()
		, m_nServoPos( 0 )
		, m_nServoMin( 0 )
		, m_nServoMax( 180 )
		, m_nServoMid( 90 )
		, m_nServoMiniMin( 70 )
		, m_nServoMiniMax( 110 )
		, m_eSweepState( CSonar::ASCEND )
		, m_nSweepLow( 400 )
		, m_nSweepHigh( -1 )
{
	setSweepLimits( m_nServoMin, m_nServoMax );
	sonarFillLow();
}

CSonar::CSonar( int nSonicTrig, int nSonicEcho, int nServoPin )
		: m_tSonic()
		, m_tServo()
		, m_nServoPos( 0 )
		, m_nServoMin( 0 )
		, m_nServoMax( 180 )
		, m_nServoMid( 90 )
		, m_nServoMiniMin( 70 )
		, m_nServoMiniMax( 110 )
		, m_eSweepState( CSonar::ASCEND )
		, m_nSweepLow( 400 )
		, m_nSweepHigh( -1 )
{
	m_tSonic.init( nSonicTrig, nSonicEcho );
	m_tServo.attach( nServoPin );
	setSweepLimits( m_nServoMin, m_nServoMax );
	m_nServoPos = m_tServo.read();
	sonarFillLow();
}

CSonar::~CSonar()
{}

void
CSonar::init( int nSonicTrig, int nSonicEcho, int nServoPin )
{
	m_tSonic.init( nSonicTrig, nSonicEcho );
	m_tServo.attach( nServoPin );
}

void
CSonar::setSweepLimits( int nMin, int nMax )
{
	m_nServoMin = max( nMin, 0 );
	m_nServoMax = min( nMax, 180 );
	DEBUG_PRINT( "Sweep: Min=" );
	DEBUG_PRINT( m_nServoMin );
	DEBUG_PRINT( "; Max=" );
	DEBUG_PRINT( m_nServoMax );
	m_nServoMid = ( m_nServoMax - m_nServoMin ) / 2;
	DEBUG_PRINT( "; Mid=" );
	DEBUG_PRINTLN( m_nServoMid );
}

void
CSonar::setMiniSweepOffset( int nOffset )
{
	nOffset = min( max( nOffset, 0 ), 80 );
	m_nServoMiniMin = m_nServoMid - nOffset;
	m_nServoMiniMax = m_nServoMid + nOffset;
}

void
CSonar::setServoPosition( int nPos )
{
	// nPos = min( max( nPos, m_nServoMin ), m_nServoMax );
	m_tServo.write( nPos );
	m_nServoPos = nPos;
}

void
CSonar::setServoMiddle()
{
	setServoPosition( m_nServoMid );
}

bool
CSonar::isServoMiddle()
{
	return m_nServoMid == getServoPosition();
}

void
CSonar::scan( int nMin, int nMax )
{
	nMin = nMin < 0 ? m_nServoMin : nMin;
	nMax = nMax < 0 ? m_nServoMax : nMax;

	if ( m_nServoPos < nMin || nMax < m_nServoPos )
	{
		if ( m_nServoPos < nMin )
		{
			m_nServoPos = nMin;
			m_eSweepState = ASCEND;
		}
		else
		{
			m_nServoPos = nMax;
			m_eSweepState = DESCEND;
		}
		m_tServo.write( m_nServoPos );
		return;
	}

	// int nPos = getServoPosition();
	// if ( nPos != m_nServoPos )
	// {
	// 	setServoPosition( m_nServoPos );
	// 	// delay( 10 );
	// 	return;
	// }
	double dist = getDistanceCm();
	if ( dist < 0.0 )
		dist = m_tSonic.lastDistanceCm();
	// DEBUG_PRINT( "Scan Pos = " );
	// DEBUG_PRINT( nPos );
	// DEBUG_PRINT( "; Member = " );
	// DEBUG_PRINTLN( m_nServoPos );
	m_aSweep[m_nServoPos] = (uint16_t)dist;
	if ( ASCEND == m_eSweepState )
	{
		m_nServoPos += 10;
		if ( nMax < m_nServoPos )
		{
			m_nServoPos = nMax;
			m_nSweepHigh = nMax;
			m_eSweepState = DESCEND;
		}
	}
	else
	{
		m_nServoPos -= 10;
		if ( m_nServoPos < nMin )
		{
			m_nServoPos = nMin;
			m_nSweepLow = nMin;
			m_eSweepState = ASCEND;
		}
	}
	setServoPosition( m_nServoPos );
}

void
CSonar::miniScan( int nMin, int nMax )
{
	nMin = nMin < 0 ? m_nServoMiniMin : nMin;
	nMax = nMax < 0 ? m_nServoMiniMax : nMax;

	scan( nMin, nMax );
}


bool
CSonar::isScanComplete( int nMin, int nMax )
{
	nMin = nMin < 0 ? m_nServoMin : nMin;
	nMax = nMax < 0 ? m_nServoMax : nMax;

	if ( nMin == m_nSweepLow && nMax == m_nSweepHigh )
		return true;
	else
		return false;
}

bool
CSonar::isMiniScanComplete( int nMin, int nMax )
{
	nMin = nMin < 0 ? m_nServoMiniMin : nMin;
	nMax = nMax < 0 ? m_nServoMiniMax : nMax;

	if ( nMin == m_nSweepLow && nMax == m_nSweepHigh )
		return true;
	else
		return false;
}

void
CSonar::sonarFillLow()
{
	m_nSweepLow = 400;
	m_nSweepHigh = -1;
	int nMax = sizeof( m_aSweep ) / sizeof( m_aSweep[0] );
	for ( int i = 0; i < nMax; ++i )
		m_aSweep[i] = 0;
}

void
CSonar::sonarFillHigh()
{
	m_nSweepLow = 400;
	m_nSweepHigh = -1;
	int nMax = sizeof( m_aSweep ) / sizeof( m_aSweep[0] );
	for ( int i = 0; i < nMax; ++i )
		m_aSweep[i] = 30000;
}

int
CSonar::getServoPosition()
{
	int nPos = m_tServo.read();
	// DEBUG_PRINT( "ServoPos=" );
	// DEBUG_PRINTLN( nPos );
	return nPos;
}


double
CSonar::getDistanceCm()
{
	return m_tSonic.getDistanceCm();
}

int
CSonar::getAngleMid()
{
	return m_nServoMid;
}

uint16_t
CSonar::getDistanceAtAngle( int n )
{
	return m_aSweep[n];
}

uint16_t
CSonar::getMaxDistance( int nMin, int nMax )
{
	uint16_t nValue = 0;
	uint16_t dist = 0;

	nMin = nMin < 0 ? m_nServoMin : nMin;
	nMax = nMax < 0 ? m_nServoMax : nMax;

	for ( int i = nMin; i <= nMax; ++i )
	{
		dist = min( m_aSweep[i], 400 );
		if ( nValue < m_aSweep[i] )
			nValue = m_aSweep[i];
	}
	return nValue;
}

uint16_t
CSonar::getMiniMaxDistance( int nMin, int nMax )
{
	nMin = nMin < 0 ? m_nServoMiniMin : nMin;
	nMax = nMax < 0 ? m_nServoMiniMax : nMax;

	return getMaxDistance( nMin, nMax );
}

uint16_t
CSonar::getMinDistance( int nMin, int nMax )
{
	uint16_t nValue = 30000;
	uint16_t dist = 0;

	nMin = nMin < 0 ? m_nServoMin : nMin;
	nMax = nMax < 0 ? m_nServoMax : nMax;

	for ( int i = nMin; i <= nMax; ++i )
	{
		dist = min( m_aSweep[i], 400 );
		if ( dist < nValue )
			nValue = dist;
	}
	return nValue;
}

uint16_t
CSonar::getMiniMinDistance( int nMin, int nMax )
{
	nMin = nMin < 0 ? m_nServoMiniMin : nMin;
	nMax = nMax < 0 ? m_nServoMiniMax : nMax;

	return getMinDistance( nMin, nMax );
}


uint16_t
CSonar::getMaxDistanceAngle( int nMin, int nMax )
{
	uint16_t nDistMax = 0;
	uint16_t nDist = 0;
	int      nAngle = m_nServoMid;


	nMin = nMin < 0 ? m_nServoMin : nMin;
	nMax = nMax < 0 ? m_nServoMax : nMax;

	for ( int i = nMin; i <= nMax; ++i )
	{
		nDist = min( max( m_aSweep[i], 1 ), 400 );
		if ( nDistMax < nDist )
		{
			nDistMax = nDist;
			nAngle = i;
		}
	}
	// debugDumpSweep();
	// Serial.print( "angle=" );
	// Serial.print( nAngle );
	// Serial.print( "; dist=" );
	// Serial.println( nDistMax );
	return nAngle;
}

void
CSonar::debugDumpSweep()
{
	for ( int i = m_nServoMin; i <= m_nServoMax; i += 2 )
	{
		Serial.print( m_aSweep[i] );
		Serial.print( " " );
	}
	Serial.println( "." );
}


#endif    // H_SONAR
