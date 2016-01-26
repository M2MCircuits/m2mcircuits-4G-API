/* Modem Interface class
 * Handles high-level functions for communicating with modem
 * #define [MICROCONTROLLER] before including ModemInterface.h
 */

#ifndef MODEMINTERFACE
#define MODEMINTERFACE

#ifdef LINUX
#include <syslog.h>
#endif

#ifndef CSTRING
#define CSTRING
//#include <cstring>
#endif

#include "ModemStream.h"

class ModemInterface 
{
  private:
	#ifdef ARDUINO
		
	#endif
	#ifdef LINUX
	
	#endif

  public:
    ModemStream* stream;
    ModemInterface(ModemStream* stream);
    ~ModemInterface();
	bool init();
	bool checkForConnection();
	bool sendCommandCheckForOkay(char* cmd);
    void sendCommand(char* cmd);
	void sendCommand(int cmd);
	void flushBuffer();
	void logDebug(char* debug, int priority);
};

#endif
