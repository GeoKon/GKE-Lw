// ----- References to classes allocated elsewhere but needed here ------------

    #include "externIO.h"       // global externals of cpu,...,server
    #include "SimpleAP.h"       // exported functions by this file

// ----------------------- Exported functions -------------------------------------

void setupAP()
{
    PRN("\r\nAP mode");
    ASSERT( WiFi.softAP("GKE_AP") );
    
    PR( "ACCESS POINT MODE. Connect to GKE_AP, browse to: "); PRN( WiFi.softAPIP());

    cpu.led( ON );                  // ON indicates AP mode
}
void loopAP()
{
    static int connected = -1;
    
    int numbconx = WiFi.softAPgetStationNum();
    if( connected != numbconx )
    {
        PF("%d connections\r\n", connected = numbconx );
        cpu.led( OFF );
        delay( 300 );
        cpu.led( ON );
    }
}
