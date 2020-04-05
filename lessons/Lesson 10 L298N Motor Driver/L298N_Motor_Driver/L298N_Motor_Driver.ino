int IN1 = 5;
int IN2 = 6;
int IN3 = 7;
int IN4 = 8;
int ENA = 9;
int ENB = 10;
void
setup()
{
	for ( int i = 5; i < 11; i++ )
	{
		pinMode( i, OUTPUT );
	}
}
void
loop()
{
	// rotate CW
	digitalWrite( IN1, LOW );
	digitalWrite( IN2, HIGH );
	analogWrite( ENA, 200 );
	digitalWrite( IN3, LOW );
	digitalWrite( IN4, HIGH );
	analogWrite( ENB, 200 );
	delay( 2000 );
	// pause for 2S
	analogWrite( ENA, 0 );
	analogWrite( ENB, 0 );
	delay( 1000 );
	// rotate CCW
	digitalWrite( IN1, HIGH );
	digitalWrite( IN2, LOW );
	analogWrite( ENA, 100 );
	digitalWrite( IN3, HIGH );
	digitalWrite( IN4, LOW );
	analogWrite( ENB, 100 );
	delay( 2000 );
	// pause for 2S
	analogWrite( ENA, 0 );
	analogWrite( ENB, 0 );
	delay( 2000 );
}
