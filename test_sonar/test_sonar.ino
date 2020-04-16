
#include "cardetails.h"
#include "debug.h"
#include "delay.h"
#include "sonar.h"

CDelay        m_tDelay;
CSonar        m_tSonar;
unsigned long g_uTimeCurrent;

void
scanner()
{
	if ( m_tDelay.timesUp( g_uTimeCurrent ) )
	{
		m_tSonar.scan();
		if ( m_tSonar.isScanComplete() )
		{
			int nDist = m_tSonar.getMaxDistance();
			int nPos = m_tSonar.getMaxDistanceAngle();
			//m_tSonar.debugDumpSweep();
			DEBUG_PRINT( "pos=" );
			DEBUG_PRINT( nPos );
			DEBUG_PRINT( "; dist=" );
			DEBUG_PRINT( nDist );
			DEBUG_PRINT( "; sweep[" );
			DEBUG_PRINT( nPos );
			DEBUG_PRINT( "]=" );
			DEBUG_PRINTLN( m_tSonar.getDistanceAtAngle( nPos ) );
			// m_tSonar.debugDumpSweep();
			m_tSonar.sonarFillLow();
		}
	}
}

void
setup()
{
	Serial.begin( 9600 );
	m_tDelay.init( 100 );
	m_tSonar.init( kPinSonicTrig, kPinSonicEcho, kPinServo );
	m_tSonar.setSweepLimits( k_nServoMinSweep, k_nServoMaxSweep );
	m_tSonar.setServoMiddle();
	m_tSonar.sonarFillLow();
}

void
loop()
{
	g_uTimeCurrent = millis();
	scanner();
}
