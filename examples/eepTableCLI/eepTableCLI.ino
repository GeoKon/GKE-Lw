#define TEST 2
/*                SELECTION OF TESTS 
 * 
 * 1 = interactive blocking demo CLI in setup() 
 * 2 = typical boot with WiFi with no user EEPROM parms
 * Use the eepGlobals.ino to test user+eep parameters
*/
#include <externIO.h>
#include <eepTable.h>
#include <FS.h>

// --------------- forward references (in this file) ---------------------------

    void interact();
    extern CMDTABLE mypTable[]; 

// ------------------- allocation of base classes ------------------------------

    CPU cpu;
    CLI cli;
    EXE exe;
    EEP eep;

// -----------------------------------------------------------------------------
#if TEST == 1        // primitive boot in CLI to test functionality
void setup()
{
    cpu.init();
    SPIFFS.begin();                     // crashes sometimes if not present
    PF("This is test #%d\r\n", TEST );
    
    exe.registerTable( mypTable );
    exe.registerTable( eepTable );
    
    interact();
}
#endif

// -----------------------------------------------------------------------------
#if TEST == 2        // typical boot with WiFi with no user EEPROM parms

#define myMAGIC 0x1234
void setup()
{
    cpu.init();
    ASSERT( SPIFFS.begin() );
    
    PF("This is test #%d\r\n", TEST );
    
    if( !eep.checkEEParms( myMAGIC, 0 ) )       // fetches parameters and returns TRUE or FALSE
    {
        PF("Initializing parms!\r\n" );
        eep.initHeadParms( myMAGIC, 0 );
        eep.initWiFiParms();                    // initialize with default WiFis
    }
    eep.incrBootCount();
    eep.printHeadParms("--- Current Parms");    // print current parms
    eep.printWiFiParms("--- WiFi Parms");                 
    
    exe.registerTable( mypTable );
    exe.registerTable( eepTable );
    
    interact();
}
#endif

void loop() 
{
    yield();
}

// ----------------------------- local CLI tables ---------------------------

    void help( int n, char *arg[] ) {exe.help( n, arg );}
    void cliEx1( int n, char **arg )
    {
        BINIT( bp, arg );
        if( bp )  bp->add("Buf Example 1\r\n");
        else      PF("Prn Example 1\r\n");
    }
    void cliEx2( int n, char **arg )
    {
        BINIT( bp, arg );
        if(  bp ) bp->add("Buf Example 2\r\n");
        else      PF("Prn Example 2\r\n");
    }
    CMDTABLE mypTable[]= 
    {
        {"h", "Help! List of all commands", help },
        {"ex1", "Example 1", cliEx1 },
        {"ex2", "Example 2", cliEx2 },
        {NULL, NULL, NULL}
    };

// -----------------------------------------------------------------------------
void interact()
{
    BUF temp(2000);                // response buffer
    
    cli.init( ECHO_ON, "cmd: " );       
    PR("CLI Mode. Press 'h' for help\r\n");
    
    cli.prompt();
    for(;;)
    {
        if( cli.ready() )
        {
            char *p = cli.gets();

//            exe.dispatchConsole( p );
//            PF("--- Repeated ---\r\n");           

            exe.dispatchBuf( p, temp );
            temp.print();
            
            cli.prompt();
        }
    }
}
