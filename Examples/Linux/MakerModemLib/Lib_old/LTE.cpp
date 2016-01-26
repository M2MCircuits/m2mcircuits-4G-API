#include "LTE.h"

using namespace std;

LTE::LTE(ModemInterface* modem)
{
	this->modem = modem;
	
	//If enabled for cell data set up collection
	#ifdef CELLDATA
		string response = modem->sendCommand("AT+UCELLINFO=1");
		#ifdef DEBUG
			if (response == "OK"){
				modem->logDebug("Cell data collection mode set to 2\n");
			}
			else {
				modem->logDebug("Cell data initialization failed\n");
			}
	#endif
}

string LTE::getManufacturer()
{
	string response = modem->sendCommand("AT+CGMI");
	return response.substr(0,response.find('\n'));
}

string LTE::getModel()
{
	string response = modem->sendCommand("AT+CGMM");
	return response.substr(0,response.find('\n'));
}

string LTE::getFirmware()
{
	string response = modem->sendCommand("AT+CGMR");
	return response.substr(0,response.find('\n'));
}

string LTE::getIMEI() 
{
	string response = modem->sendCommand("AT+CGSN=0");
	return response.substr(0,response.find('\n'));
}

string LTE::getICCID() 
{
	string response = modem->sendCommand("AT+CCID");
	return response.substr(response.find(' '),response.find('\n')-reponse.find(' '));
}

//This method asks for the cell data from the LTE module
//Returns "MCC,MNC,LAC,CELLID"
string LTE::getCellData()
{
	//ask for cell data
	string response = modem->sendCommand("AT+UCELLINFO?");
	//parse out mcc, mnc, and lac
	string retVal = "";
	int i = 0;
	while (i < 6) {
		if (i == 2) {
			retVal += "MCC: " + reponse.substr(0,reponse.find(','));
		}
		else if (i == 3) {
			retVal += "MNC: " + reponse.substr(0,reponse.find(','));
		}
		else if (i == 4) {
			retVal += "LAC: " + reponse.substr(0,reponse.find(','));
		}
		else if (i == 5) {
			retVal += "CI: " + reponse.substr(0,reponse.find(','));
			return retVal;
		}
		reponse = reponse.substr(reponse.find(','),reponse.find(',',reponse.find(',')));
		i++;
	}
}

void LTE::basicInfoDump()
{
	modem->logDebug("Manufacturer: " + getManufacturer());
	modem->logDebug("Model: " + getModel());
	modem->logDebug("Firmware: " + getFirmware());
	modem->logDebug("IMEI: " + getIMEI());
	modem->logDebug("ICCID: " + getICCID());
}
