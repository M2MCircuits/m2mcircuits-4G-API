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
