#pragma once

#include <ESP8266WebServer.h>
#include <bufClass.h>
	
// ------- Exported functions from SimpleSVR.cpp ----------------------------------

    extern char *navigate;
	extern char *Landing_STA_Page;
	extern char *Landing_AP_Page;
	extern char *Landing_Help_Page;
    
	void cliCallbacks( ESP8266WebServer &server, BUF &buffer );
	void srvCallbacks( ESP8266WebServer &server, char *landingpage );
	
    void setTrace( int value );
    int getTrace();
    
    #define T_REQUEST 1
    #define T_JSON    2
    #define T_ARGS    4
    #define T_FILEIO  8
	#define T_ACTIONS 16

    void showArgs( );
    void showJson( String &json );
	void showJson( char *s );
    
    String fileList( bool br = false );
    String formatBytes(size_t bytes);
    String getContentType(String filename);
	bool handleFileRead(String path);
