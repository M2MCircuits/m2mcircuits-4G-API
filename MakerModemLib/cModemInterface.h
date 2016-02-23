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

#include <string.h>

#include "cModemStream.h"

struct ModemInterface {
	struct ModemStream* modem;
	int dlmode;
	int disconnectIndex;
};
#ifdef LAUNCHPAD
/*
* TODO: Launchpad
*/
#endif

#ifdef ARDUINO
bool mmSendCommandCheckForOkay(struct ModemInterface* interf, FlashStringHelper cmd);
void mmSendCommand(struct ModemInterface* interf, FlashStringHelper cmd);	
struct ModemInterface* mmCreate(Stream* stream);
#endif
#ifdef LINUX
struct ModemInterface * mmCreate(char* blkdvc);
#endif
#ifdef _WIN32

struct ModemInterface * mmCreate(char* blkdvc);

#endif


bool mmInit(struct ModemInterface* interf);
bool mmCheckForConnection(struct ModemInterface* interf);
bool mmCheckForOkay(struct ModemInterface* interface);
bool mmSendCommandCheckForOkay(struct ModemInterface* interf, char* cmd);
void mmSendCommand(struct ModemInterface* interf, char* cmd);
//void mmSendCommand(struct ModemInterface* interf, int cmd);
int mmGetResponse(struct ModemInterface* interf, char* buffer, int length);
void mmFlushBuffer(struct ModemInterface* interf);
void mmLogDebug(char* debug, int priority);

#endif
