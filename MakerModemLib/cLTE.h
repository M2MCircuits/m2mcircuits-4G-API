/* LTE Class
 * Gives general information about the module
 * Also provides cell location capability
 */

#ifndef LTE_H
#define LTE_H

#include "cModemInterface.h"
#include <string.h>

//This variable is used in waiting for i/o response from modem
int milliTimeoutLTE = 50;

int mmGetManufacturer(struct ModemInterface* interface, char* buffer, int length);
int mmGetModel(struct ModemInterface* interface, char* buffer, int length);
int mmGetFirmware(struct ModemInterface* interface, char* buffer, int length);
int mmGetIMEI(struct ModemInterface* interface, char* buffer, int length);
int mmGetICCID(struct ModemInterface* interface, char* buffer, int length);
bool mmGetCellData(struct ModemInterface* interface, char* buffer, int length);

#endif
