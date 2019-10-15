#include <ESP8266WiFi.h>
#include <externIO.h>
#include <SimpleSTA.h>
#include <ticClass.h>

CPU cpu;
CLI cli;
EXE exe;
EEP eep;

TICsec tic(30);
TICsec tic1(1);

void setup() 
{
	int cnt = 0;
	cpu.init();
	startCLIAfter( 10 );		
	setupWiFi( "thomcape", "shesabho", "" );
	PRN("Entering main loop()");
}
void loop() 
{
	if( isWiFiConnected() && reconnectWiFi() )
	{
		if( tic1.ready() )
			PRN("I am connected OK");
				
		if( tic.ready() )								// simulate WiFi disconnect
		{
			PRN("Disconnecting");
			WiFi.disconnect();
		}
	}
	else // wifi disconnected
	{
		reconnectWiFi( NO_CHANGES, NULL );					// stay in this state until reconnection
		// LED will blink here
	}
}
