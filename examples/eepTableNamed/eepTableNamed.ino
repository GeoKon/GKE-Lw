/* ---------------------------------------------------------------------------
 *  Example of a simple WEB application. Should be used as a template
 *  to create other applications.
 *  
 *  Use/Modify the ExampleSTA.cpp to include the specific STA callbacks
 *  Use/Modify the ExampleAP.cpp to include AP mode functionality
 *  The above should include SimpleSRV.cpp with the baseline WEB functions
 *  --------------------------------------------------------------------------
 */
    #include <FS.h>
    #include "Globals.h"            // in this local directory

// ---------- allocation of classes used in this application ------------------
    
    CPU cpu;
    CLI cli;
    EXE exe;
    EEP eep;
    
// ----------------------------- Main Setup -----------------------------------
void setup() 
{
    cpu.init(); 
    ASSERT( SPIFFS.begin() );

    myp.initAllParms( 0x1647 );

    myp.gp.itest = 1;
    myp.gp.ftest = 2;
    strcpy( myp.gp.stest, "value3" );
    myp.saveMyEEParms();    
    myp.fetchMyEEParms();
    myp.printMyEEParms("Updated Parms");

    nmp.setParmByStr( "itest", "1000" );
    nmp.setParmByStr( "ftest", "2000.56" );
    myp.saveMyEEParms();    
    myp.fetchMyEEParms();
    myp.printMyEEParms("Updated Parms");

    
    ASSERT( false );    
}

void loop()
{
    ;
}
