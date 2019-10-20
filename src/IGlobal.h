/* ----------------------------------------------------------------------------------
 *
 *		INTERFACE CLASS FOR GLOBAL PARAMETERES
 *
 *  Copyright (c) George Kontopidis 1990-2019 All Rights Reserved
 *  You may use this code as you like, as long as you attribute credit to the author.
 * ---------------------------------------------------------------------------------
 */
#pragma once

// =================== Minimum necessary includes for this module ===================

#include <setjmp.h>
#include "externIO.h"       // in GKE-Lw. Includes and externs of cpu,...,eep

// --------------- Definition of Global parameters ---------------------------------
    
    class IGlobal	// Abstract class. Contains pure virtual functions, mixed with implementations
    {
      public:
		~IGlobal(){;}											// See https://www.tutorialcup.com/cplusplus/interfaces.htm
																// ======= A1. Add here all volatile parameters
		jmp_buf env;											// The main() entry longjmp environment
		
		virtual void initVolatile() = 0;                      	// ======= A2. Initialize here the volatile parameters
		virtual void printVolatile( char *prompt="", 
									BUF *bp=NULL )=0; 			// ======= A3. Add to buffer (or print) all volatile parms
		struct gp_t                                         	// ======= B1. Add here all non-volatile parameters into a structure
		{                           
		} gp;													// this structure will be expanded by the definition
		
		virtual void initMyEEParms()=0;                         // ======= B2. Initialize here the non-volatile parameters
        virtual void registerMyEEParms()=0;                     // ======= B3. Register parameters by name
        virtual void printMyEEParms( char *prompt="", 
									BUF *bp=NULL )=0; 			// ======= B4. Add to buffer (or print) all volatile parms
		virtual void initAllParms()=0;
		
	 protected:
        byte *bpntr;                                        	// pointer to User EEPROM structure
        int bsize;                                          	// byte count of the User EEPROM structure
	 
	 public:
																// Initialialize volatile parms. Fetch eeprom parms. If bad, fix the EEPROM.
		void initTheseParms(int myMagic, byte *upntr, int usize)                  
		{
			bpntr = upntr;
			bsize = usize;
			
			initVolatile();                                 	// initialize volatile parameters
			registerMyEEParms();                            	// always associate names with data structure
			
			if( !eep.checkEEParms( myMagic, bsize ) )       	// fetches Header & WiFi & User parms (if any). Returns TRUE if OK
			{
				PF("=== Initializing parms!\r\n" );
				eep.initHeadParms( myMagic, bsize );        	// initialize header parameters AND save them in eeprom
				eep.initWiFiParms();                        	// initialize with default WiFi AND save them in eeprom
				
				initMyEEParms();                            	// initialize user EEPROM parameters
				saveMyEEParms();								// save user EEPROM parameters
			}
			PF("Fetching %d user parms\r\n", bsize ); 
			eep.fetchUserStruct( bpntr, bsize );            	// from EEPROM to working memory area   
			eep.incrBootCount();                            	// number of times system is booted
			eep.printHeadParms("--- Current Head Parms");       // print current parms
			eep.printWiFiParms("--- Current WiFi Parms");    
			printMyEEParms    ( "--- User EEPROM Parms");
		}
	public:
		void fetchMyEEParms()
		{
			PF("Fetching %d-bytes of user parms\r\n", bsize ); 
			eep.fetchUserStruct( bpntr, bsize );            	// from EEPROM to working memory area     
		}
		void saveMyEEParms()
		{
			PF("Saving %d-bytes of user parms\r\n", bsize );
			eep.saveUserStruct( bpntr, bsize );    
		}
	};