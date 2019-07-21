#pragma once
/* ---------------------------------------------------------------------------------
 *          GLOBAL Classes and Structures
 * ----------------------------------------------------------------------------------
 */

#include <externIO.h>           // external definitions and headers for cpu..eep
#include <nmpClass.h>
extern NMP nmp;

// ---------------------- Definition of the Global class ----------------------------

class Globals
{
public:                                                 // ======= A1. Add here all volatile parameters 
    
    void initVolatile()                                 // ======= A2. Initialize here the volatile parameters
    {
        ;
    }    
    void printVolatile( char *prompt="", BUF *bp=NULL ) // ======= A3. Add to buffer (or print) all volatile parms
    {
        ;
    }
    struct gp_t                                         // ======= B1. Add here all non-volatile parameters into a structure
    {                           
        int   itest;                
        float ftest;
        char  stest[USER_STR_SIZE]; 
    } gp;
    
    void initMyEEParms()                                // ======= B2. Initialize here the non-volatile parameters
    {
        gp.itest = 10;
        gp.ftest = 20.32;
        strcpy( gp.stest, "String Example" );
    }
    void registerMyEEParms()                            // ======= B3. Register parameters by name
    {
        nmp.resetRegistry();
        nmp.registerParm( "itest", 'd', &gp.itest, "= %d max value" ); 
        nmp.registerParm( "ftest", 'f', &gp.ftest, "= %5.3f 3dec digits" );
        nmp.registerParm( "stest", 's',  gp.stest, "= %s ok" );
        PF("%d named parameters registed\r\n", nmp.nparms );
        ASSERT( nmp.getSize() == sizeof( gp_t ) );     
    }
    void printMyEEParms( char *prompt="", BUF *bp=NULL ) // ======= B4. Add to buffer (or print) all volatile parms
    {
        nmp.printParms( prompt );
    }
    #include <GLOBAL.hpp>                               // Common code for all Global implementations
    
//    void initAllParms( int myMagic  )       
//    void fetchMyEEParms()
//    void saveMyEEParms()
};

// Exported class
extern Globals myp;                                     // allocated by Globals.cpp       
  
