#pragma once
#include <bufClass.h>
#include <externIO.h>

enum recon_t
{
	INITIALIZE = 0,
	RECONNECT = 1,
	WIFI_OK = 2
};

#define WAITFOR_CLI 		"$*."
#define WAITFOR_WIFI 		"$*...."
#define WAITFOR_SMARTCONFIG "$*..*........"
#define WAITFOR_IPADDR		"$*****.."
#define WAITFOR_RECONN		"$*......."

// CLI Exported functions 

	bool startCLIAfter( int timeoutsec, BUF *bp=NULL );
    void interactForever( BUF *bp=NULL );  // requires cli.init();

// WiFi Connections (use these)
	
	bool isWiFiConnected();
	void setupWiFi( char *ssid=eep.wifi.ssid, 
					char *pwd=eep.wifi.pwd, 
					char *staticIP=eep.wifi.stIP );

	bool checkWiFi();
	void reconnectWiFi( void (*cb)()=NULL,
					    const char *pattern=WAITFOR_RECONN);				
	
	char *getDevName( char *subname="" );
	bool startMDNS();

    void loopSTA();

// WiFi Connections	(deprecated)
    // bool setupSTA( int tmout = 20 );
	// bool connectToNetwork( int tmout, void (*func)() = [](){delay(250);} );
	// bool printOnce( char *s );				// use NULL to initialize. Include in a loop
	// void indicateDisconnect( bool doit );  	// blinks LED if WiFi disconnected

