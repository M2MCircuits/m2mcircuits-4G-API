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
