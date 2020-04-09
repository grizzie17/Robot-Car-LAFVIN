#ifndef H_QUEUE
#define H_QUEUE


enum QueueTypes
{
	Q_NULL,
	Q_STOP,
	Q_FORWARD,    // arg=speed
	Q_REVERSE,    // arg=speed
	Q_TURNRIGHT,
	Q_TURNLEFT,
	Q_SONICSTOP,
	Q_RADARSCAN
};

typedef struct QueueData
{
	QueueTypes _type;
	int        _arg;

	QueueData()
			: _type( Q_NULL )
			, _arg( 0 )
	{}
	QueueData( QueueTypes type )
			: _type( type )
			, _arg( 0 )
	{}
	QueueData( QueueTypes type, int arg )
			: _type( type )
			, _arg( arg )
	{}
	QueueData( QueueData& r )
			: _type( r._type )
			, _arg( r._arg )
	{}
} QueueData;

// circular queue
class Queue
{
public:
	Queue();
	virtual ~Queue();

	bool
	pop( QueueData* pData );

	bool
	append(                     ///@returns: status of append
			QueueData* pData    ///< [in] pointer to new data
	);

protected:
#define k_QueueSize 16
	QueueData m_aQ[k_QueueSize];
	int       m_nSize;
	int       m_nFront;
	int       m_nRear;
};

Queue::Queue()
		: m_nSize( k_QueueSize )
		, m_nFront( -1 )
		, m_nRear( -1 )
{}

Queue::~Queue()
{}


bool
Queue::append( QueueData* pData )
{
	if ( ! pData )
		return false;
	if ( ( 0 == m_nFront && m_nRear == m_nSize - 1 )
			|| ( m_nRear == ( m_nFront - 1 ) % ( m_nSize - 1 ) ) )
		return false;

	if ( -1 == m_nFront )    // first element
	{
		m_nFront = m_nRear = 0;
		m_aQ[m_nRear] = *pData;
	}
	else if ( m_nRear == m_nSize - 1 && 0 != m_nFront )
	{
		m_nRear = 0;
		m_aQ[m_nRear] = *pData;
	}
	else
	{
		++m_nRear;
		m_aQ[m_nRear] = *pData;
	}
	return true;
}

bool
Queue::pop( QueueData* pData )
{
	if ( ! pData )
		return false;
	if ( -1 == m_nFront )
		return false;

	*pData = m_aQ[m_nFront];
	if ( m_nFront == m_nRear )
	{
		m_nFront = -1;
		m_nRear = -1;
	}
	else if ( m_nFront == m_nSize - 1 )
	{
		m_nFront = 0;
	}
	else
	{
		++m_nFront;
	}
	return true;
}


#endif    // H_QUEUE
