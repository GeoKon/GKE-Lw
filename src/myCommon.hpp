// Contains segments of often used code

#ifdef MISCSETUP_COMMON

    void blinkLED( int ms, uint32_t dly=0 );                // forward reference in this file
    
	void setupSTA()
	{
		PR("Starting STA mode...\r\nMAC=");
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

		while( WiFi.status() != WL_CONNECTED  )         // fast blink while waiting for WiFi connection
			blinkLED( 200 );

		PR("IP=");PRN( WiFi.localIP() );
		
		byte mac[6];
		WiFi.macAddress( mac );
		B64 name;
		name.set("GKE-%02x-%02x", mac[0], mac[5] );     // MSB and LSB of mac address
		if (!MDNS.begin( !name )) 
			cpu.die("Error setting up MDNS responder!", 3 );
		PF("mDNS advertising: %s.local, Port:%d\r\n", !name, eep.wifi.port ); 
			
		setTrace( T_REQUEST | T_JSON );  
		srvCallbacks( server, Landing_STA_Page );  
		cliCallbacks( server, clirspbuf  );
		snfCallbacks();                                 
	 
		server.begin( eep.wifi.port );
		PRN("HTTP server started. Hit RETURN to activate CLI during normal operation\r\n");
	}

    // void initializeWorld()
    // {
        // myp.registerMyParms();                              // register named parameters once for all
        
        // if( !eep.checkEEParms( myMAGIC, myp.bsize ) )      // fetches parameters and returns TRUE or FALSE
        // {
            // PF("=== Initializing parms!\r\n" );
            // eep.initHeadParms( myMAGIC );                   // this sets user_size to zero, 
            //                                                ... but modified to myp.bsize by eep.setUserStruct()
            // eep.initWiFiParms();                            // initialize with default WiFi
            // myp.initMyParms();                              // initialize named parameters and save them in EEPROM
            ser.printParms("AFTER INITIALIZATION");
        // }
        // eep.incrBootCount();
        // myp.fetchMyParms();                                 // retrieve parameters from EEPROM
        // eep.printHeadParms("--- Current Parms");            // print current parms
        // eep.printWiFiParms();                 
        // ser.printParms("--- User Parms");
    
        // exe.registerTable( mypTable );               // register tables to CLI
        // exe.registerTable( eepTable );
    // }
    
    // void interactForever()
    // {
        // cli.init( ECHO_ON, "cmd: " );       
        // PRN("CLI Mode. Press 'h' for help\r\n");
        
        // cli.prompt();
        // for(;;)
        // {
            // if( cli.ready() )
            // {
                // char *p = cli.gets();
                // if( strcmp( p, "exit" ) == 0 )
                    // break;
              // exe.dispatchConsole( p );
              // PF("--- Repeated ---\r\n");
                
                // BUF temp(2000);                // RESPONSE1 buffer
                // exe.dispatchBuf( p, temp );
                // temp.print();
                
                // cli.prompt();
            // }
        // }
        // PF("Exited CLI\r\n");
    // }

    // Call repeately. Does not block. Blinks ms-ON, ms-OFF. Optional delay at the end.
    void invokeTmoutCLI( int timeout )
    {
        cli.init( ECHO_ON, "cmd: " );       
    
        PF("Press RETURN to start CLI within %d sec\r\n", timeout );
        for( int i=0; i< timeout*100; i++ )
        {
            blinkLED( 100 );                                // fast blink to indicate waiting for RETURN
            if( Serial.read() == 0x0D )
            {
                interactForever();
                break;                                      // break the loop if 'exit' is entered    
            }
        }   
    }
    void blinkLED( int ms, uint32_t dly )
    {
        static uint32_t T0=0;
        uint32_t msL = ms;
        static bool toggle = false;
        
        if( millis()-T0 > msL )     // most likely true the first time
        {
            toggle = !toggle;
            cpu.led( (onoff_t) toggle );
            T0 = millis();          
        }
        if( dly==0 )
            dly = msL/10;
        delay( dly );
    }    



#undef MISCSETUP_COMMON
#endif

// --------------------- Conditional Includes ---------------------------------
#ifdef MYPTABLE_COMMON
	#define RESPONSE1( A, ... )   PF(A, ##__VA_ARGS__);
	#define BINIT1(A,B) // nothing

    void setUserParm( int n, char **arg )                   // do not modify this!
    {
        BINIT1( bp, arg );
		if( n<2 )
        { RESPONSE1("Use <name> <value>\r\n"); return;}

        int type = ser.getParmType( arg[1] );
        if( type==0 )
        {			
		    RESPONSE1("Parm %s not found\r\n", arg[1]);
        }
		else
        {
            ser.setParmByStr( arg[1], arg[2] );                    
            RESPONSE1("OK. Use 'usave' after you are done\r\n");
        }
    }
	void showUserParms( int n=0, char **arg=NULL );
    void showUserParms( int n, char **arg )                   // do not modify this!
    {
        BINIT1( bp, arg );
        for( int i=0; i<ser.nparms; i++ )
            RESPONSE1( "%s\r\n", ser.getParmString(i).c_str() );
    }
    
	void cliShowStatus( int n=0, char **arg=NULL );
	void cliShowStatus( int n, char **arg )
    {
        BINIT1( bp, arg );
         BUF s(512);
        s.init();
		addWiFiStatus( &s, false );
		RESPONSE1( !s );
    }
	void addWiFiStatus( BUF *s, bool htmlEOL )
	{
        BINIT1( bp, arg );
 		char *termT = "<br/>\r\n";		
		char *termF = "\r\n\t";
		char *term = htmlEOL ? termT: termF;
		
		if( !htmlEOL )
			s->add("\t");
		
		s->add(" CPU: %04x%s", ESP.getChipId(), term );
		s->add("Core: %s%s",   ESP.getCoreVersion().c_str(), term);
			
		uint8_t m[6];
		WiFi.macAddress( m );
		s->add(" MAC: %02x:%02x:%02x:%02x:%02x:%02x%s", m[0],m[1],m[2],m[3],m[4],m[5], term );

		IPAddress ipa; 
		ipa = WiFi.localIP();        
		s->add("  IP: %s%s",  		ipa.toString().c_str(), term );
		
		s->add("DHCP: %s%s",     	WiFi.hostname().c_str(), term );
		s->add("SSID: %s%s",  		WiFi.SSID().c_str(), term );
		s->add(" PWD: %s%s",   		WiFi.psk().c_str(), term );
		s->add("Chan: %d%s",   		WiFi.channel(), term );
		s->add("RSSI: %d dBm%s",		WiFi.RSSI(), term );
		s->add("HeapUse: %d KB%s", 	cpu.heapUsedNow()/1024, term );
		s->add("HeapMax: %d KB%s", 	cpu.heapUsedMax()/1024, term );
	}
	
	void cliTestOut45( int n=0, char **arg=NULL );
    void cliTestOut45( int n, char **arg )                         // displays list of files  
    {
        BINIT1( bp, arg );
        pinMode(4, OUTPUT);
        pinMode(5, OUTPUT);
        for(int i=0;;i++)
        {
            cpu.led( ON );
            digitalWrite( 4, true );
            digitalWrite( 5, false);
            delay( 400);
            cpu.led( OFF );
            digitalWrite( 4, false);
            digitalWrite( 5, true );
            delay( 400);
            PF("You must reset after this %d\r\n", i);
        }
    }
	void cliTestInputs( int n=0, char **arg=NULL ); 
	void cliTestInputs( int n, char **arg )                        // displays list of files  
    {
        BINIT1( bp, arg );
 		pinMode(14,INPUT);
		pinMode(2, INPUT);
		pinMode(4, INPUT);
		pinMode(5, INPUT);
		char *hi = "*H*";
		char *lo = "-L-";
		
		for(;;)                            // TEST of the IO pins
		{
		   PF("GPIO2=%s GPIO4=%s GPIO5=%s GPIO14=%s\r\n",
		   digitalRead( 2 )	?hi:lo,
		   digitalRead( 4 )	?hi:lo,
		   digitalRead( 5 )	?hi:lo,
		   digitalRead( 14 )?hi:lo );
		   delay(400 );
	   }
    }
	void cliShowFiles( int n=0, char **arg=NULL );
	void cliShowFiles( int n, char **arg )  
	{
        BINIT1( bp, arg );
 		Dir dir = SPIFFS.openDir("/");
		bool empty = true;
		while (dir.next()) 
		{    
			empty=false;
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			RESPONSE1( "\t%s\t%dkB\r\n", fileName.c_str()+1, fileSize/1024 );
		}  
		if( empty) RESPONSE1("No files found!\r\n");
	}

#undef MYTABLE_COMMON
#endif

#ifdef ROOTSTATUS_COMMON
    // server.on("/",
    // [](){
        // server.send(200, "text/html",
        // "<h1 align=\"center\">STA Services<br/></h1>"
        // "<h2 align=\"center\">"
        // "<a href=\"status\">Show WiFi status</a><br/><br/>"
        // "<a href=\"files\">Show Files</a><br/><br/>"
        // "<a href=\"show\">Show Parameters</a><br/><br/>"
		// "<a href=\"cli\">Use REST cli</a><br/><br/>"
		// "<a href=\"trace\">Show web trace</a><br/><br/>"
 
        // "<a href=\"index.htm\">Goto INDEX</a></h2>"
        // );
    // });
    // server.on("/status", HTTP_GET, 
    // [](){
        // showArgs();
        // BUF s(512);

        // s.set("<h3 align='center'>\r\n");
        // mypTable::addWiFiStatus( &s, true );

        // s.add("</h3>\r\n");
        // s.add( navigate );
     
        // showJson( !s );  
        // server.send(200, "text/html", !s);
    // }); 
	// server.on("/cli", HTTP_GET,              // when command is submitted, webcli.htm is called  
    // [](){
        // showArgs();
		// clirspbuf.set("Use /cli?cmd=<args>");
        // if( server.args() )                                 // if command given
        // {
            // clirspbuf.init();                                 // initialize the RESPONSE1 buffer
            // B64 cmd;
			// cmd.copy( server.arg(0).c_str() );
			// exe.dispatchBuf( !cmd, clirspbuf );  // command is executed here and RESPONSE1 is saved in 'cliresp' buffer
        // }        
		// showJson( !clirspbuf );                           // show the same on the console
        // server.send(200, "text/plain", !clirspbuf );
    // });
#undef ROOTSTATUS_COMMON
#endif

#ifdef SETSHOWPARM_COMMON
    server.on("/set", HTTP_GET, 
    [](){
        showArgs();
        BUF resp(128);
        BUF final(128);
            
        if( server.args() )                              // if variable is given
        {
            char cmd[80];
            sprintf( cmd, "uset %s %s", !server.argName(0), !server.arg(0) );

            resp.init();                      
            exe.dispatchBuf( cmd, resp );          // command is executed here and RESPONSE1 is saved in 'resp' buffer

            final.set("<h3 align='center'>\r\n");
            final.add("%s</h3>\r\n", !resp );
            final.add( navigate );
        }
        showJson( !final );                           // show the same on the console
        server.send(200, "text/html", !final );
    });
    server.on("/show", HTTP_GET, 
    [](){
        showArgs();
        BUF s(512);

        s.set("<h3 align='center'>\r\n");

        for( int i=0; i<ser.nparms; i++ )
            s.add("%s<br/>\r\n", ser.getParmString(i).c_str() );

        s.add("<br/>(Use '/set?parm=value' to modify)<br/>");
        s.add("</h3>\r\n");
        s.add( navigate );
        showJson( !s );                              
        server.send(200, "text/html", !s );
    });
#undef SETSHOWPARM_COMMON
#endif

