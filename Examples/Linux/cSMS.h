/* SMS class
 * Provides SMS functionality
 */

#ifndef SMS
#define SMS

#include <string.h>
#include <stdbool.h>

#include "cModemInterface.h"
#include "cModemStream.h"

int sendTextMessage(struct ModemInterface* interf, char * number, char* buffer);
int getTextLists(struct ModemInterface* interf, int* ids, int length);
int retrieveTextMessage(struct ModemInterface* interf, char* buffer, int length, int id);
int retrievePhoneNumber(struct ModemInterface* interf, char* buffer, int length, int id);
int retrieveTimestamp(struct ModemInterface* interf, char* buffer, int length, int id);
int deleteTextMessage(struct ModemInterface* interf, int id);

#endif
