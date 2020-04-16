#ifndef H_DEBUG
#define H_DEBUG

#define _DEBUG

#if defined( _DEBUG )

#	define DEBUG_PRINT( s )   Serial.print( s )
#	define DEBUG_PRINTLN( s ) Serial.println( s )

#else

#	define DEBUG_PRINT( s )   ( 0 )
#	define DEBUG_PRINTLN( s ) ( 0 )

#endif    // _DEBUG

#endif    // H_DEBUG
