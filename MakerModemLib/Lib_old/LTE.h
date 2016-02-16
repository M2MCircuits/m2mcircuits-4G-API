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
#ifndef LTE_H
#define LTE_H


#ifndef MODEMINTERFACE
#include "ModemInterface.h"
#endif


//MakerModem includes


//General Library Includes
#include <string>

/*Defines different options for the LTE module*/

//If defined - Prints debug messages to the log
#define DEBUG
//If defined - Allows collection of cell data (for cell locates)
#define CELLDATA

class LTE
{
	private:
		ModemInterface* modem;
	public:
		LTE(ModemInterface* modem);
		~LTE();
		string getManufacturer();
		string getModel();
		string getFirmware();
		string getIMEI();
		string getICCID();
		string getCellData();
		void basicInfoDump();
};
#endif
