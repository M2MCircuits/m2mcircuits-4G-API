/*
    This file is part of the MakerModem API.

    The MakerModem API is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The MakerModem API is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the MakerModem API.  If not, see <http://www.gnu.org/licenses/>.
*/

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
