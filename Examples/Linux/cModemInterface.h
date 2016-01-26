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


int mmInit(struct ModemInterface* interf);
int mmCheckForConnection(struct ModemInterface* interf);
int mmSendCommandCheckForOkay(struct ModemInterface* interf, char* cmd);
void mmSendCommand(struct ModemInterface* interf, char* cmd);
//void mmSendCommand(struct ModemInterface* interf, int cmd);
int mmGetResponse(struct ModemInterface* interf, char* buffer, int length);
void mmFlushBuffer(struct ModemInterface* interf);
void mmLogDebug(char* debug, int priority);

#endif