    
	// Insert this code in the Global class!
    
    byte *bpntr;                                        // pointer to User EEPROM structure
    int bsize;                                          // byte count of the User EEPROM structure

    void initAllParms( int myMagic  )                  // Initialialize volatile parms. Fetch eeprom parms. If bad, fix the EEPROM.
    {
        bpntr = (byte *)&gp;
        bsize = sizeof( gp_t );
        
        initVolatile();                                 // initialize volatile parameters
        registerMyEEParms();                            // always associate names with data structure
        
        if( !eep.checkEEParms( myMagic, bsize ) )       // fetches Header & WiFi & User parms (if any). Returns TRUE if OK
        {
            PF("=== Initializing parms!\r\n" );
            eep.initHeadParms( myMagic, bsize );        // initialize header parameters AND save them in eeprom
            eep.initWiFiParms();                        // initialize with default WiFi AND save them in eeprom
            
            initMyEEParms();                            // initialize user EEPROM parameters
            saveMyEEParms();
        }
        PF("Fetching %d user parms\r\n", bsize ); 
        eep.fetchUserStruct( bpntr, bsize );            // from EEPROM to working memory area   
        eep.incrBootCount();                            // number of times system is booted
        eep.printHeadParms("--- Current Head Parms");       // print current parms
        eep.printWiFiParms("--- Current WiFi Parms");    
        printMyEEParms    ( "--- User EEPROM Parms");
    }
    void fetchMyEEParms()
    {
        PF("Fetching %d-bytes of user parms\r\n", bsize ); 
        eep.fetchUserStruct( bpntr, bsize );            // from EEPROM to working memory area     
    }
    void saveMyEEParms()
    {
        PF("Saving %d-bytes of user parms\r\n", bsize );
        eep.saveUserStruct( bpntr, bsize );    
    }