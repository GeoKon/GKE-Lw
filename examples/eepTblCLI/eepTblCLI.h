#pragma once
#include <cliClass.h>
static EXE *_exe;

class DEMO
{
public:

    DEMO(  EXE &exe ) 
	{ 
		_exe = &exe;
	}
    static void help( int n, char *arg[] ) 
    {
        _exe->help( n, arg );
//        exe.help( n, arg );
    }    
    static void cliEx1( int n, char **arg )
    {
        PF("Example 1\r\n");
    }
    static void cliEx2( int n, char **arg )
    {
        PF("Example 2\r\n");
    }
	static CMDTABLE table[];
};

// intialize the static table within the class
CMDTABLE DEMO::table[]= 
{
	{"h", "Help! List of all commands", DEMO::help },
	{"ex1", "Example 1", DEMO::cliEx1 },
	{"ex2", "Example 2", DEMO::cliEx2 },
	{NULL, NULL, NULL}
};
