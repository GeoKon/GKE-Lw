#include <externIO.h>
#include <eepTable.h>

// ----------------------- CLI Handlers ---------------------------------------------
    
    #define DEFBP( bp ) BUF *bp = (BUF *)arg[0]
    #define RESPONSE( A, ... ) if( bp )                     \
                                bp->add(A, ##__VA_ARGS__ ); \
                               else                         \
                                PF(A, ##__VA_ARGS__);
    
    static void cliEEInit( int n, char **arg )
    {
        DEFBP( bp );
        eep.initHeadParms();  
        eep.initWiFiParms();          // initializes ALL structures and writes them to EEPROM
        RESPONSE("OK\r\n");
    }
    static void cliShowParms( int n, char **arg )
    {
        DEFBP( bp );
        RESPONSE( eep.getHeadString().c_str() );
        RESPONSE( eep.getWiFiString().c_str() );
    }   
    static void cliFetchParms( int n, char **arg )
    {
        DEFBP( bp );
        eep.fetchHeadParms();         
        eep.fetchWiFiParms();     
        RESPONSE( eep.getHeadString().c_str() );    
        RESPONSE( eep.getWiFiString().c_str() );
    }   
    static void cliUpdateWiFi( int n, char **arg )
    {
        DEFBP( bp );
        eep.fetchWiFiParms();
        
        if( n>1 )
            eep.initWiFiParms( arg[1], eep.wifi.pwd, eep.wifi.stIP, eep.wifi.port );
        if( n>2 )
            eep.initWiFiParms( arg[1], arg[2], eep.wifi.stIP, eep.wifi.port );
        if( n>3 )
            eep.initWiFiParms( arg[1], arg[2], arg[3], eep.wifi.port );
        if( n>4 )    
            eep.initWiFiParms( arg[1], arg[2], arg[3], atoi( arg[4] ) );
        
        eep.saveWiFiParms();
        RESPONSE( eep.getWiFiString().c_str() );
    }
    static void cliUpdatePort( int n, char **arg )
    {
        DEFBP( bp );
        eep.fetchWiFiParms();
        
        int port = n>1 ? atoi( arg[1] ) : 80;
        eep.initWiFiParms( NULL, NULL, NULL, port );
        RESPONSE( "Port=%d\r\n", eep.wifi.port );
    }
    static void cliUpdateCount( int n, char **arg )
    {
        DEFBP( bp );
        eep.fetchHeadParms();
        if( n>1 )
        {
            eep.head.reboots = atoi( arg[1] );
            eep.saveHeadParms();
        }
        RESPONSE( "Reboot Counter=%d\r\n", eep.head.reboots );
    }
    static void cliStatus( int n, char **arg )
    {
        DEFBP( bp );
		
		uint8_t m[6];
		WiFi.macAddress( m );
		IPAddress ipa; 
		ipa = WiFi.localIP();  
		
		char *format = 
			"\tCore: %s\r\n"
			"\t MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n"
			"\t  IP: %s\r\n"
			"\tDHCP: %s\r\n";
			
		RESPONSE( format, 
			ESP.getCoreVersion().c_str(),
			m[0],m[1],m[2],m[3],m[4],m[5],
			ipa.toString().c_str(),
			WiFi.hostname().c_str() );
			
		format =
				"\tSSID: %s\r\n"
				"\t PWD: %s\r\n"
				"\tChan: %d\r\n"
				"\tRSSI: %d dBm\r\n"
				"\tHeapUse: %d KB\r\n"
				"\tHeapMax: %d KB\r\n";
		
		RESPONSE( format,
			WiFi.SSID().c_str(),
			WiFi.psk().c_str(),
			WiFi.channel(),
			WiFi.RSSI(), 
			cpu.heapUsedNow()/1024,
			cpu.heapUsedMax()/1024 );

        // RESPONSE( "RSSI %ddBm, Heap now=%d, Heap max=%d\r\n", WiFi.RSSI(), cpu.heapUsedNow(), cpu.heapUsedMax() );
    }
    CMDTABLE eepTable[]= // must be external to be able to used by the cliSupport
    {        
        {"wifi",    "[ssid|*] [pwd|*] [staticIP|*] [port]. Updates WiFi parms", cliUpdateWiFi },
        {"status",  "Shows RSSI and Heap use", 			                       cliStatus },
		{"port",   "port. Updates EEPROM port. Reboot!",                       cliUpdatePort },

        {"show",   "Shows EEPROM (header and WiFi)",                           cliShowParms  },
        {"einit",  "Initializes default Header and WiFi EEPROM parameters",    cliEEInit },
        {"fetch",  "Fetches and displays EEPROM",                              cliFetchParms },        
        {"bcount", "[value] prints or sets reboot counter",                    cliUpdateCount },
        
        {NULL, NULL, NULL}
    };


// -------------------------- DIAGNOSTICS ---------------------------------------------------
#if DIAGNOSTICS
	static void cliEEDump( int n, char **arg )     // edump base [N]
	{
		int base = (n>1) ? atoi( arg[1] ) : 0;
		int    N = (n>2) ? atoi( arg[2] ) : HEAD_PSIZE + WIFI_PSIZE + _eep->USER_PSIZE;

		// prints a list of memory locations in HEX or ASCII.
		EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + _eep->USER_PSIZE );
		int i;
		char c;
		
		for( i=0; i<N; i++ )
		{
			EEPROM.get( base+i, c );
			if( (c>=' ') && (c<0x7f) )
				RESPONSE(" %c ", c );
			else
				RESPONSE( "%02X ", c );
			if( (i%20)==19 )
				RESPONSE("\r\n");
		}
		if( (i%20)!=0)
			RESPONSE("\r\n");
		EEPROM.end();
	}
	static void cliEEZero( int n, char **arg )     // edump base [N]
	{
		// prints a list of memory locations in HEX or ASCII.
		EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + _eep->USER_PSIZE );
		int i, j;
		char c = 0;
		
		j = HEAD_PSIZE;
		for( i=0; i<WIFI_PSIZE+_eep->USER_PSIZE; i++ )
			EEPROM.put( j++, c ); 
		RESPONSE("All WiFi and User Parms Zeroed\r\n");
		EEPROM.end();
	}

	static void synerr( char *name, int n )
	{
		RESPONSE( "%s requires at least %d arguments", name, n);
	}

	static void cliEEPut( int n, char **arg )     // eset base value8bit
	{
		if( n<=2 )
		{synerr( arg[0], 2 ); return;}
		
		int addr = atoi( arg[1] );
		char patt = atoi( arg[2] );

		EEPROM.begin( HEAD_PSIZE+WIFI_PSIZE+_eep->USER_PSIZE );
		EEPROM.put( addr, patt );
		EEPROM.end();  
	}    
	
	// direct to EEPROM. No changes to structures
		{"edump",   "[base=0] [count=all]. Prints EEPROM contents",    cliEEDump },
		{"ezero",   "Zeros all EEPROM contents",    cliEEZero },
		{"eput",    "base value. Sets an EEPROM location",    cliEEPut },

#endif
