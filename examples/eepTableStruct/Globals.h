#pragma once
/* ---------------------------------------------------------------------------------
 *          GLOBAL Classes and Structures
 * ----------------------------------------------------------------------------------
 */

#include <externIO.h>           // external definitions and headers for cpu..eep

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
        char stest[20]; 
    } gp;
    
    void initMyEEParms()                                // ======= B2. Initialize here the non-volatile parameters
    {
        gp.itest = 10;
        gp.ftest = 20.32;
        strcpy( gp.stest, "String Example" );
    } 
    void registerMyEEParms()                            // ======= B3. Register parameters by name
    {   
        ;
    }
    void printMyEEParms( char *prompt="", BUF *bp=NULL ) // ======= B4. Add to buffer (or print) all volatile parms
    {
        PF( "%s\r\n", prompt );
        PF( "itest\t= %d\r\n", gp.itest );
        PF( "ftest\t= %f\r\n", gp.ftest );
        PF( "stest\t= %s\r\n", gp.stest );
    }    
    #include <GLOBAL.hpp>                               // Common code for all Global implementations
    
//    void initAllParms( int myMagic  )       
//    void fetchMyEEParms()
//    void saveMyEEParms()
};

// Exported class
extern Globals myp;                                     // allocated by Globals.cpp       
  
