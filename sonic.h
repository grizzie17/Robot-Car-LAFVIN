//===============================================
/// @file: sonic.h
//===============================================

#ifndef H_SONIC
#define H_SONIC

// #ifdef MFG_LAFVIN
// #include <HCSR04.h>
// #endif

//-----------------------------------------------
// CSonic class
//-----------------------------------------------
class CSonic
{
public:
	CSonic();
	virtual ~CSonic();

public:
	void
	init( int nTrigger, int nEcho );

	double
	getDistanceCm();

	double
	lastDistanceCm();

protected:
	double
	CmFromMicroseconds( unsigned long duration );

	const unsigned long PULSE_TIMEOUT = 150000L;    // 100ms

	int    m_nPinTrigger;
	int    m_nPinEcho;
	double m_dLastDistance;
};


//-----------------------------------------------
// CSonic code
//-----------------------------------------------
CSonic::CSonic()
		: m_nPinTrigger( 0 )
		, m_nPinEcho( 0 )
		, m_dLastDistance( 0.0 )
{}

CSonic::~CSonic()
{}

void
CSonic::init( int nTrigger, int nEcho )
{
	m_nPinTrigger = nTrigger;
	m_nPinEcho = nEcho;

	pinMode( m_nPinTrigger, OUTPUT );
	pinMode( m_nPinEcho, INPUT );
}

double
CSonic::getDistanceCm()
{
	digitalWrite( m_nPinTrigger, LOW );
	delayMicroseconds( 2 );
	digitalWrite( m_nPinTrigger, HIGH );
	delayMicroseconds( 10 );
	digitalWrite( m_nPinTrigger, LOW );
	delayMicroseconds( 2 );
	unsigned long uEchoTime = pulseIn( m_nPinEcho, HIGH );
	double        dDist = CmFromMicroseconds( uEchoTime );
	if ( 0.0 < dDist && dDist < 500.0 )
	{
		return dDist;
	}
	else
	{
		return -1.0;
	}
}

double
CSonic::CmFromMicroseconds( unsigned long duration )
{
	// const double kSpeedOfSound
	// 		= 0.03313 + 0.0000606 * 19.307;  // Cair
	// (331.3 + 0.606 * temperature) double dDist =
	// double( duration ) / 4.0 * kSpeedOfSound; return
	// dDist;

	double d = double( duration ) / 58.82;
	if ( 0.0 < d )
		m_dLastDistance = d;

	return m_dLastDistance;
}

double
CSonic::lastDistanceCm()
{
	return m_dLastDistance;
}

#endif    // H_SONIC
