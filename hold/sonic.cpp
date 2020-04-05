//===============================================
/// @file: sonic.cpp
//===============================================


#include "sonic.h"


CSonic::CSonic() : m_nPinTrigger( 0 ), m_nPinEcho( 0 ), m_pSensor( 0 )
{}

CSonic::~CSonic()
{
	delete m_pSensor;
}

void
CSonic::init( int nTrigger, int nEcho )
{
	m_nPinTrigger = nTrigger;
	m_nPinEcho = nEcho;

	if ( ! m_pSensor )
	{
		m_pSensor = new UltraSonicDistanceSensor( m_nPinTrigger, m_nPinEcho );
	}
}

double
CSonic::getDistanceCm()
{
	return m_pSensor->measureDistanceCm();
}
