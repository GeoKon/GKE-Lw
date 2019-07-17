// Required include for all GLE-Lw classes
// Main application should instantiate these classes, with the specific names below.

#pragma once

    #include <cpuClass.h>
    #include <cliClass.h>
    #include <eepClass.h>
    #include <ESP8266WebServer.h>

    extern CPU cpu;                     // base IO functions, defined in cpuClass.cpp
    extern CLI cli;                     // base CLI functions, defined in cliClass.cpp
    extern EXE exe;                     // base Dispatcher (EXE) functions, defined in cliClass.cpp
    extern EEP eep;                     // base EEPROM functions, defined in eepClass.cpp

    extern ESP8266WebServer server;
