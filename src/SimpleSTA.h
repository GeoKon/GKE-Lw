#pragma once

// ------ Expected definitions/allocations in main() ------------------------------
  
// Exported functions by ExampleSTA.cpp

	bool startCLIAfter( int timeoutsec, BUF *bp=NULL );
    void interactForever( BUF *bp=NULL );
	
    void setupSTA();
    void loopSTA();
	
	