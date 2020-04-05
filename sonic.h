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

	void
	init( int nTrigger, int nEcho );

	double
	getDistanceCm();

protected:
	int m_nPinTrigger;
	int m_nPinEcho;
};


//-----------------------------------------------
// CSonic code
//-----------------------------------------------
CSonic::CSonic() : m_nPinTrigger( 0 ), m_nPinEcho( 0 )
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
	unsigned long uEchoTime = pulseIn( m_nPinEcho, HIGH );

	const double kSpeedOfSound
			= 0.03313 + 0.0000606 * 19.307;  // Cair (331.3 + 0.606 * temperature)
	double dDist = uEchoTime / 2.0 * kSpeedOfSound;
	if ( 0 < dDist && dDist < 500 )
	{
		return dDist;
	}
	else
	{
		return -1.0;
	}
}

#endif
