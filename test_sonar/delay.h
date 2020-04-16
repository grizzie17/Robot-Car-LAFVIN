#ifndef H_TIMER
#define H_TIMER


class CDelay
{
public:
	CDelay();
	CDelay( unsigned long nMilliSeconds );

public:
	void
	init( unsigned long nMilliSeconds );

	void
	newDelay( unsigned long nMilliSeconds );

	void
	reset();

	bool
	timesUp( unsigned long nCurrentTime );

protected:
	unsigned long m_nDelay;
	unsigned long m_nPreviousTime;
};


CDelay::CDelay()
		: m_nDelay( 250 )
		, m_nPreviousTime( 0 )
{}

CDelay::CDelay( unsigned long nMilliSeconds )
		: m_nDelay( nMilliSeconds )
		, m_nPreviousTime( 0 )
{}

void
CDelay::init( unsigned long nMilliSeconds )
{
	m_nDelay = nMilliSeconds;
	m_nPreviousTime = 0;
}

void
CDelay::newDelay( unsigned long nMilliSeconds )
{
	m_nDelay = nMilliSeconds;
}

void
CDelay::reset()
{
	m_nPreviousTime = 0;
}

bool
CDelay::timesUp( unsigned long nCurrentTime )
{
	if ( m_nDelay < nCurrentTime - m_nPreviousTime )
	{
		m_nPreviousTime = nCurrentTime;
		return true;
	}
	else
	{
		return false;
	}
}


#endif    // H_TIMER
