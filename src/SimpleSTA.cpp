// -------- Choose one of the two options below ---------------------------------

//  #define MULTI_WIFI		
	#undef  MULTI_WIFI

// -------------------------------------------------------------------------------

    #include <ESP8266WiFi.h> 
	#define TEST_CONNECTION WiFi.status()
	#define SETUP_WIFI(A,B) WiFi.begin(A,B)
    
    #include <ESP8266mDNS.h>        // Include the mDNS library

    #include "externIO.h"           // global externals of cpu,...,server
    #include "ledClass.h"
	#include "SimpleSTA.h"          // exported functions by this file    
	
// ------------------- blinking LED pattern -----------------------------------------
	
	LED led;						// allocate the LED structure

// ----------------------- Console interface ----------------------------------------

static void doSmartConfig()								// waits for mobile app credentials
{														//   and updates EEPROM
	PRN("Attempting Smart Config" );	
	led.pattern( WAITFOR_SMARTCONFIG );					// define smart conf. LED pattern
	WiFi.beginSmartConfig();
   
	while( led.blink( !WiFi.smartConfigDone() ))		// wait forever for smart conf. app
		;
	ASSERT( led.aborted() );							// Smart config OK
	
	strcpy( eep.wifi.ssid, WiFi.SSID().c_str() );		// update EEPROM parms
	strcpy( eep.wifi.pwd,  WiFi.psk().c_str() );
	eep.saveWiFiParms();
	PRN("Smartconf OK. WiFi Credentials saved in EEPROM");
}

/* 
 *  Waits for 'timeout' for either button-press or RETURN to be pressed.
 *  While in this loop, blinks LED fast.
 *  If RETURN is pressed, enters CLI, with LED ON. If the command "exit" is entered
 *    this routine returns with 'false'
 *  If Button is pressed, activates the Smart Config, until new credentials are
 *    entered using the Mobile App. If new credentials entered, they are saved in
 *    EEPROM and returns with 'true'. 
 */

bool startCLIAfter( int timeout, BUF *bp )
{
	cli.init( ECHO_ON, "cmd: " );    
	
	PFN("Press RETURN within %d sec to start CLI", timeout );
	
	led.pin( cpu.getLedPin() );				// define the LED pin
	led.pattern( WAITFOR_CLI, timeout);		// define the blinking timeout
	
	while( led.blink( Serial.read() != 0x0D ) )
	{
		if( cpu.buttonPressed() )			// if button is pressed...
		{
			doSmartConfig();				// wait until smart config is done
			led.stop();
			return true;					
		}
	}	
	if( led.expired() )						
		return false;
	
	if( led.aborted() )
	{
		cpu.led( ON );
		interactForever( bp );
		cpu.led( OFF );
		PRN("Continuing Setup()...");      // break the loop if 'exit' is entered 
		return false;
	}
}

void interactForever( BUF *bp )
{
    PF("\r\nCLI Mode. Press 'h' for help\r\n");
    
    cli.prompt();
    
    for(;;)
    {
        if( cli.ready() )
        {
            char *cmd = cli.gets();
			if( strcmp( cmd, "exit")==0 )
				break;
            if( bp == NULL )
				exe.dispatchConsole( cmd );		// process with printf()
			else
			{
				bp->init();						// process using supplied buffer
				exe.dispatchBufPtr( cmd, bp );
				bp->print();
			}
            cli.prompt();
        }
    }
}

// --------------------- new WiFi connect with Smart Connect -----------------------

bool isWiFiConnected()
{
    if( WiFi.status() == WL_CONNECTED) 
    {
        if( WiFi.localIP() != INADDR_NONE )
            return true;
    }
    return false;
} 

/* 
 * Connects to WiFi. If no connection is made within 30sec, resets the processor.
 * While is waiting for connection, the LED blinks slowly.
 */
void setupWiFi( char *ssid, char *pwd, char *staticIP )
{
	// Force clearing the previous settings
	// see: https://circuits4you.com/2018/03/09/esp8266-static-ip-address-arduino-example/
	
	WiFi.begin( ssid, pwd ); 
	
	WiFi.disconnect( /*true*/ );	// use this to clear past connections
	if( *staticIP )
    {
        IPAddress ip, gateway(192,168,0,1), subnet( 255,255,255,0 ), dns(8, 8, 8, 8);
        ip.fromString( staticIP );
        PR( " with static IP="); PRN( ip );
        WiFi.config( ip, gateway, subnet, dns );
    }
   	WiFi.begin( ssid, pwd ); 
  	WiFi.mode(WIFI_STA);
  	
	PFN("Attempting to connect to SSID:%s with PWD:%s", ssid, pwd );
	
	led.pin( cpu.getLedPin() );
	led.pattern( WAITFOR_WIFI, 30 );						// 30 second timeout
	
	while( led.blink( WiFi.status() != WL_CONNECTED ) )
		;
	
	if( led.expired() )										// Reset the processor if no connection
		ESP.restart();
	
	PFN( "Hostname:%s, SSID:%s, PWD:%s, RSSI:%ddBm", 
		WiFi.hostname().c_str(), 
		WiFi.SSID().c_str(), 
		WiFi.psk().c_str(), 
		WiFi.RSSI() );

  	//WiFi.printDiag(Serial);

  	PRN("Getting IP Address" );			
  	led.pattern( WAITFOR_IPADDR );							// wait for ever 
  	while( led.blink(WiFi.localIP() == INADDR_NONE) )
  		;
	ASSERT( led.aborted() );								
	PR( "IP Address: " ); PRN( WiFi.localIP() );
	
	PRN( "Connected OK" );
	ASSERT( isWiFiConnected() );
}
/* 
Use the following to test:
   1. start application with valid SSID. If not, it will do SmartConfig()
   2. in the main loop change SSID to invalid
   3. do a 'disconnect'; this will start the checkWiFi() loop
   4. ensure that switch ON/OFF work OK
   5. set a valid SSID; reconnection should happen.
*/

static int state = 0;			
/* 	
	Shared variable between checkWiFi() and reconnectWiFi()

	0 = idle state. WiFi is connected.
	1 = just disconnected. Start reconnecting sequence.
	2 = test if now connected. Wait until connection is OK
*/
bool checkWiFi()
{
	if( isWiFiConnected() )
	{
		if( state != 0 )		// this indicates that reconnectWiFi() was connected OK
		{
			PF("\r\nPrev state %d. Reconnected with IP=", state );
			Serial.println(WiFi.localIP());
		}
		state = 0;
		return true;
	}	
	else						// WiFi is not connected
	{
		if( state == 0 )		// If in idle state,
		{
			state = 1;			//  start the reconnect sequence
			CRLF();
		}	
		return false;
	}
}	
void reconnectWiFi( void (*cb)(), const char *pattern )
{
	switch( state )
	{
		default:
		case 0:										// do nothing
			break;
		
		case 1: 
			WiFi.begin( eep.wifi.ssid, eep.wifi.pwd );  
		  	WiFi.mode(WIFI_STA);
			PFN("Attempting to reconnect to SSID:%s PWD:%s", 
				eep.wifi.ssid, eep.wifi.pwd );
			led.pattern( pattern, 10 );				// with 10 sec timeout			
			state = 2;
			break;
		case 2:
			if( cb )								// callback if not null
				(*cb)();
			if( led.blink( !isWiFiConnected() )	)	// stay in this state if not connected
				break;
		  	if( led.expired() )						// repeat the attempt in 10sec
				state = 1;
		  	else //if( led.aborted() )				// WiFi is connected here
		  	{
				PR("Reconnected. IP Address is ");
				Serial.println(WiFi.localIP());
				state = 0;
			}
			break;									// reconnection is done
	}
}

char *getDevName( char *subname )
{
	static char name[20];
	if( subname != NULL )
	{
		byte mac[6];
		WiFi.macAddress( mac );
		sf( name, 20, "Gke%s-%02X%02X", subname, mac[0], mac[5] );     // MSB and LSB of mac address
	}
	return name;		// if NULL, it returns the previously set name
}
bool startMDNS()
{
	byte mac[6];
	char *name = getDevName();
	WiFi.macAddress( mac );
	bool ok = MDNS.begin( name ) ;
	if( ok )
		PF("mDNS advertising: %s.local:%d\r\n", name, eep.wifi.port ); 
	else
		PF("mDNS could not start!\r\n" );
	return ok;	
}

void loopSTA()
{
    if( cli.ready() )               // handle serial interactions
    {
        char *p = cli.gets();
        exe.dispatchConsole( p );
        cli.prompt();                
    }      
}
