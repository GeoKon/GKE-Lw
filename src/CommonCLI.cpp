/* ----------------------------------------------------------------------------------
 *								Common CLI handlers 
 *
 *  This module depends on externIO.h (cpu, cli, exe) and IGlobal parameter interface.
 *  The main program must call "linkParms2cmnTable()" to make the link with Globals
 *
 *  Copyright (c) George Kontopidis 1990-2019 All Rights Reserved
 *  You may use this code as you like, as long as you attribute credit to the author.
 * ---------------------------------------------------------------------------------
 */
// Common CLI handlers for TmpHumdSensor.cpp and SenseGWY.cpp

// Minimum necessary for this file to compile

#include <FS.h>
#include <externIO.h>       // IO includes and cpu...exe extern declarations
#include "IGlobal.h"		// interface class to the global parameters
#include "SimpleSRV.h"      // We only need the getWiFiStatus()
#include "CommonCLI.h"
#include "nmpClass.h"

extern NMP nmp;

static char *channel = "";
#define BINIT( A, ARG )    BUF *A = (BUF *)ARG[0]

#define RESPONSE( A, ... ) if( bp )                     \
                            bp->add(A, ##__VA_ARGS__ ); \
                           else                         \
                            PF(A, ##__VA_ARGS__);

// ------------------------------------------------------------------------------
static IGlobal *gbl;
void linkParms2cmnTable( IGlobal *p )		// link this module to global parameters						
{
	gbl = p;
}
// ----------------------------- CLI Command Handlers ---------------------------

    static void help ( int n, char **arg ){exe.help ( n, arg );}    // what to do when "h" is entered
    static void brief( int n, char **arg ){exe.brief( n, arg );} 
    
    // ============================== LOW LEVEL DIAGNOSTIC COMMANDS ===============================
 
    static bool kbabort()
    {
        if( Serial.available() )
            if( Serial.read() == 0x0D )
                return true;
        return false;
    }
    static void cliTestOut45( int n, char **arg )                         
    {
        PRN("GPIO4, D1, CLK: ON(250ms), OFF(1000ms)");
        PRN("GPIO5, D2, SDA: ON(1000ms), OFF(250ms)");
        
        pinMode(4, OUTPUT);
        pinMode(5, OUTPUT);
        
        while( !kbabort() )
        {
            cpu.led( ON );
            digitalWrite( 4, true );
            digitalWrite( 5, false);
            delay( 250 );
            cpu.led( OFF );
            digitalWrite( 4, false);
            digitalWrite( 5, true );
            delay( 1000);
            PR(".");
        }
        CRLF();
    }
    static void cliTestInputs( int n, char **arg )                        // displays list of files  
    {
        pinMode(14,INPUT);
        pinMode(2, INPUT);
        pinMode(4, INPUT);
        pinMode(5, INPUT);
        char *hi = "*H*";
        char *lo = "-L-";
        
        while( !kbabort() )
        {
           PF("GPIO2=%s GPIO4=%s GPIO5=%s GPIO14=%s\r\n",
           digitalRead( 2 ) ?hi:lo,
           digitalRead( 4 ) ?hi:lo,
           digitalRead( 5 ) ?hi:lo,
           digitalRead( 14 )?hi:lo );
           delay( 400 );
       }
    }
    static void cliInputPIN( int n, char **arg )                        
    {
        if( n<=1 )
        {
            PRN("? Missing PIN");
            return;
        }
        char *hi = "*H*";
        char *lo = "-L-";

        int pin = atoi( arg[1] );
        pinMode( pin, INPUT);
        
        bool tnew, told;
        tnew = digitalRead( pin );
        told = !tnew;
        while( !kbabort() )
        {
           tnew = digitalRead( pin );
           if( tnew != told )
                PF("GPIO%d=%s\r\n", pin, tnew ? hi : lo );
           cpu.led( (onoff_t) tnew );
           told = tnew;
           delay( 100 );
       }
    }
    static void cliOutputPIN( int n, char **arg )                        
    {
        static bool toggle = false;
        if( n<=1 )
        {
            PRN("?Missing PIN");
            return;
        }
        int pin = atoi( arg[1] );
        pinMode( pin, OUTPUT );
        
        while( !kbabort() )
        {
           toggle = !toggle;
           cpu.led( (onoff_t) toggle );
           digitalWrite( pin, toggle );
           delay( 500 );
       }
    }
    static void cliFormat( int n, char **arg )                        
    {
        SPIFFS.format();
        PF("OK\r\n");
    }
    static void cliDirectory( int n, char **arg )                        
    {
        Dir dir = SPIFFS.openDir("/");
        while (dir.next()) 
        {
            Serial.print(dir.fileName());
            File f = dir.openFile("r");
            Serial.println(f.size());
        }
    }

// ============================== MEGUNO LINK INTERFACE for EEPROM ===============================

    static void cliShowSystemStatus( int n, char **arg )
    {
        BINIT( bp, arg );
        if( exe.cmd1stUpper() )
		{
			// construct the meguno message
			bp->set("{UI:CONFIG|SET|status.Text=");
			bufWiFiStatus( bp, false );                    // included in SimpleSRV.cpp
			bp->add( "}\r\n" );
		}
		else
			bufWiFiStatus( bp, false );                 // included in SimpleSRV.cpp
    }

// =========================== PARAMETER (EEPROM) MANAGEMENT ==============================

    static void cliInitEEParms( int n, char **arg )       // initialize all EEPROM parms and save them
    {
        BINIT( bp, arg );
		eep.initHeadParms();        				// initialize header parameters AND save them in eeprom
		eep.saveHeadParms();

		eep.initWiFiParms();
		eep.saveWiFiParms();
				
		gbl->initMyEEParms();        
		gbl->saveMyEEParms();

        if( exe.cmd1stUpper() )
		{
			nmp.printMgnInfo( channel, "", "EE Parms" );     
			eep.printWiFiParms( channel ); 
			nmp.printMgnAllParms( channel );
		}
		RESPONSE("Initialized\r\n");        
    }
    static void cliShowWiFiParms( int n, char **arg )
    {
        BINIT( bp, arg );          
        eep.printWiFiParms("", bp);      
    }
    static void cliShowAllParms( int n, char **arg )
    {
		if( exe.cmd1stUpper() )
		{
			nmp.printMgnInfo( channel, "", "All EEP Parms" );     
			
			PF( "{TABLE:CONFIG|SET|WIFI PARMS| |---------------------------- }\r\n");
			eep.printMgnWiFiParms( channel ); 
			PF( "{TABLE:CONFIG|SET|USER PARMS| |---------------------------- }\r\n");
			nmp.printMgnAllParms( channel );
		}
		else
        {
			BINIT( bp, arg );          
			eep.printWiFiParms("", bp);      
			nmp.printAllParms("", bp );          
		}
    }
    static void cliShowUserParms( int n, char **arg )
    {            
        BINIT( bp, arg );
        int N = nmp.getParmCount();
        
		for( int i=0; i<N; i++ )
        {
            RESPONSE( "%s=%s%s", nmp.getParmName(i), nmp.getParmValueStr(i), (i==N-1)?"\r\n":", " );
        }
    }
    static void cliGetUserParm( int n, char **arg )
    {
        BINIT( bp, arg );        
        if( n<=1 )
        {
            RESPONSE("? Missing parm name");
        }
        else
        {
            //int N = nmp.getParmCount();
            for( int i=1; i<n; i++ )
            {
                RESPONSE( "%s=%s%s", arg[i], nmp.getParmValueStr( arg[i] ), (i==n-1)?"\r\n":", " );
            }
        }
    }
    
	// ----------------------- Generic Routine to Save Any Parameter ----------------------------
	
	static bool setAnyEEParm( int n, char **arg, bool meg )
    {        
        BINIT( bp, arg );
		if( n<3 )
        {
            RESPONSE("? Missing <name> <value>\r\n" );
            return false;
        }
		char *parm = arg[1];
		char *value = arg[2];
		
        if( eep.setWiFiParm( parm, value ) )            // if WiFi parm found, modify & save in EEPROM
        {
            if( meg )
                eep.printMgnWiFiParms( channel ); 
            RESPONSE( "%s updated\r\n", parm ); 
            return true;
        }
        if( !nmp.setParmByStr( parm, value ) )          // if EEPROM User parm not found
        {
            if( meg )
                nmp.printMgnInfo( channel, parm, "is unknown" );
            RESPONSE( "%s not found\r\n", parm );
            return false;
        }        
        // Here, parameter has been updated. Save in EEPROM, update Meguno, and 
        //  selectively call initialization functions.
        
        gbl->saveMyEEParms();                            // save to EEPROM
        if( meg )
        {
            nmp.printMgnParm( channel, parm );              // update the table
            nmp.printMgnInfo( channel, parm, "updated" );   // update the INFO  
        }
        RESPONSE( "%s updated\r\n", parm ); 
        return true;
    }	
    static void cliSetAnyParm( int n, char **arg )
    {
		setAnyEEParm( n, arg, exe.cmd1stUpper() );
	}


// ============================== CLI COMMAND TABLE =======================================

    CMDTABLE cmnTable[]= 
    {
        {"h",       "[mask]. Lists of all commands",                    help },
        {"b",       "[mask]. Brief help",                               brief },
        {"w",       "Shows WiFi parameters (brief)",                    cliShowWiFiParms },
        {"wifi",    "[or Wifi]. Displays wifi and system status",       cliShowSystemStatus },
		{"p",       "Shows user parameters (brief)",                    cliShowUserParms },
		{"parms",   "[or Parms]. Show all eeprom parameters",   		cliShowAllParms }, 
		
        {"test45",  "Flip-flops (LED) (GPIO4) (GPIO5) until CR",        cliTestOut45 },
        {"testinp", "Inputs GPIOs: 02, 04, 05, 14 until CR",            cliTestInputs },
        {"inpPIN",  "p1 p2 ... pN. Inputs PINS until CR",               cliInputPIN },
        {"outPIN",  "pin. Squarewave output to 'pin' until CR",         cliOutputPIN },        

        {"format",  "Formats the filesystem",                           cliFormat }, 
        {"dir",     "List files in FS",                                 cliDirectory }, 
        
        {"initEE",  "Initialize parameters to defaults and save to EEP",cliInitEEParms },

        {"set",     "[or Set] name value. Update parm & save in EEPROM",cliSetAnyParm },
        {"get",     "name1..nameN. Get parameter values",               cliGetUserParm },

        { "restart", "Reboot CPU (longjmp)",        [](int, char**){ PRN("Restarting!"); longjmp( gbl->env, 1 ); } },
        { "disc", 	"Disconnect WiFi (diag)",       [](int, char**){ PRN("Disconnecting!");WiFi.disconnect(); }   }, 
		
        {NULL, NULL, NULL}
    };
#undef RESPONSE
#undef BINIT


