/* SMS class
 * Provides SMS functionality
 */

#ifndef SMS
#define SMS

#include <string.h>

#include "cModemInterface.h"

//extern int milliTimeoutSMS = 50;

bool mmSendTextMessage(struct ModemInterface* interface, char *num, char* buffer);
int mmGetTextLists(struct ModemInterface* interface, int* ids, int length);
int mmRetrieveTextMessage(struct ModemInterface* interface, char* buffer, int length, int id);
bool mmRetrievePhoneNumber(struct ModemInterface* interface, char* buffer, int length, int id);
bool mmRetrieveTimestamp(struct ModemInterface* interface, char* buffer, int length, int id);
bool mmDeleteTextMessage(struct ModemInterface* interface, int id);

#endif
