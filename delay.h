#ifndef H_TIMER
#define H_TIMER


class CDelay
{
public:
	CDelay();
	CDelay( unsigned long nMicroSeconds );

	void
	init( unsigned long nMicroSeconds );

	bool
	timesUp( unsigned long nCurrentTime );

protected:
	unsigned long m_nDelay;
	unsigned long m_nPreviousTime;
};


CDelay::CDelay() : m_nDelay( 250 ), m_nPreviousTime( 0 )
{}

CDelay::CDelay( unsigned long nMicroSeconds ) : m_nDelay( nMicroSeconds )
{}

void
CDelay::init( unsigned long nMicroSeconds )
{
	m_nDelay = nMicroSeconds;
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


#endif  // H_TIMER
