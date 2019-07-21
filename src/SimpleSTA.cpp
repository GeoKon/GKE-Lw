// -------- Minimum includes for this module --------------------------------------

    
    #include <ESP8266mDNS.h>        // Include the mDNS library

    #include "externIO.h"           // global externals of cpu,...,server
    #include "SimpleSTA.h"          // exported functions by this file    

// ----------------------- Console interface ----------------------------------------
void interactForever()
{
    PF("\r\nCLI Mode. Press 'h' for help\r\n");
    cpu.led( ON );
    
    cli.prompt();
    
    for(;;)
    {
        if( cli.ready() )
        {
            char *cmd = cli.gets();
			if( strcmp( cmd, "exit")==0 )
				break;
            exe.dispatchConsole( cmd );
            cli.prompt();
        }
    }
}
bool startCLIAfter( int timeout )
{
	cli.init( ECHO_ON, "cmd: " );       

	PF("Press RETURN to start CLI within %d sec\r\n", timeout );

	for( int i=0; i< timeout*100; i++ ) 	// 100 loops per second
	{
		cpu.toggleEvery( 100 );             // fast blink to indicate waiting for RETURN
		if( (i%100)==0 ) 
			PR(".");
		if( Serial.read() == 0x0D )
		{
			CRLF();
			interactForever();
			PRN("Continuing Setup()...");
			break;                          // break the loop if 'exit' is entered    
		}
		if( cpu.button() )
            return true;
		delay( 10 );
	}   
	return false;
}

// ----------------------- Exported functions -------------------------------------

#define WIFI_TIMEOUT_SEC 5

void setupSTA()
{
    byte mac[6];
	
	PR("\r\nSTA mode...\r\n\tMAC=");
	PRN( WiFi.macAddress() );                   // mac is used later to define mDNS
	
    WiFi.mode(WIFI_STA);

    char *staticIP = eep.wifi.stIP;
    if( *staticIP )
    {
        IPAddress ip, gateway(192,168,0,1), subnet( 255,255,255,0 );
        ip.fromString( staticIP );
        WiFi.config( ip, gateway, subnet );
    }
    WiFi.begin( eep.wifi.ssid, eep.wifi.pwd );
    
    for( int i=0; (i<WIFI_TIMEOUT_SEC*2)||(WIFI_TIMEOUT_SEC==0); i++  )
    {
        if( WiFi.status() == WL_CONNECTED )
        {
            PF( "Connected to %s. Hostname is %s. ", WiFi.SSID().c_str(), WiFi.hostname().c_str() );
            PR( "IP Address " ); PRN( WiFi.localIP() );
            //PR( fileList() );
            break;
        }
        delay(250);
        if( i%2 ) cpu.led( ON );
        else cpu.led( OFF );
    }    
    // BUF name(16);
    // name.set("GKELAB%d", eep.head.reboots );
    // if (!MDNS.begin( !name )) 
        // cpu.die("Error setting up MDNS responder!", 3 );
    // MDNS.addService("GKETCP", "tcp", eep.wifi.port );
    // MDNS.addService("GKEUDP", "udp", 5353);
    // PF("mDNS responder started. Ping for %s.local:%d\r\n", !name, eep.wifi.port); 
	
	B64 name;
	WiFi.macAddress( mac );
	name.set("GKE%02X%02X", mac[0], mac[5] );     // MSB and LSB of mac address
	ASSERT( MDNS.begin( !name ) ) ;
	PF("mDNS advertising: %s.local:%d\r\n", name.c_str(), eep.wifi.port ); 
    
    cpu.led( BLINK, 2 );            // indicates successful WiFi connection 
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

//#include <ESP8266HTTPClient.h>
//HTTPClient http;                    // Object of class HTTPClient
//void loopSTA()
//{
//    static bool pending = false;
//    
//    if( cli.ready() )               // handle serial interactions
//    {
//        //exe.dispatch( cli.gets() );
//        char *p = cli.gets();
//        http.begin( p );            // "http://192.168.0.70:8067/status");
//        pending = true;
//    }
//    if( pending )
//    {
//        int httpCode = http.GET();
//        if (httpCode > 0) 
//        {
//            String buffer;
//            buffer = http.getString();
//            PF("Received: %s\r\n", !buffer );
//            pending = false;
//            http.end(); //Close connection
//            cli.prompt();
//        }
//    }
//    server.handleClient();    
//}
