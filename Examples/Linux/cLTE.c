#include "cLTE.h"

int mmGetManufacturer(struct ModemInterface* interface, char* buffer, int length)
{
	mmFlushBuffer(interface);
	mmSendCommand(interface, F("AT+CGMI\r"));
	sleep_millis(milliTimeoutLTE);
	return mmGetResponse(interface, buffer, length);
}
int mmGetModel(struct ModemInterface* interface, char* buffer, int length)
{
	mmFlushBuffer(interface);
	mmSendCommand(interface, F("AT+CGMM\r"));
	sleep_millis(milliTimeoutLTE);
	return mmGetResponse(interface, buffer, length);
}
int mmGetFirmware(struct ModemInterface* interface, char* buffer, int length)
{
	mmFlushBuffer(interface);
	mmSendCommand(interface, F("AT+CGMR\r"));
	sleep_millis(milliTimeoutLTE);
	return mmGetResponse(interface, buffer, length);
}
int mmGetIMEI(struct ModemInterface* interface, char* buffer, int length) 
{
	mmFlushBuffer(interface);
	mmSendCommand(interface, F("AT+CGSN\r"));
	sleep_millis(milliTimeoutLTE);
	return mmGetResponse(interface, buffer, length);
}
int mmGetICCID(struct ModemInterface* interface, char* buffer, int length) 
{
	mmFlushBuffer(interface);
	mmSendCommand(interface, F("AT+CCID\r"));
	sleep_millis(milliTimeoutLTE);
	return mmGetResponse(interface, buffer, length);
}


//This method asks for the cell data from the LTE module
//Returns "MCC,MNC,LAC,CELLID"
bool mmGetCellData(struct ModemInterface* interface, char* buffer, int length)
{
	mmFlushBuffer(interface);
	mmSendCommand(interface, F("AT+UCELLINFO?\r"));
	sleep_millis(milliTimeoutLTE);
	
	char interBuf[20];
	bool error;
	int bytes;
	int position;
	
	error = mmFindChar(interface->modem,',');
	error = mmFindChar(interface->modem,',');
	if (!error) {
		return false;
	}
	
	//MCC
	bytes = mmReadBytesUntil(interface->modem,',',interBuf,length);
	strncat(buffer,interBuf,bytes);
	
	//MNC
	bytes = mmReadBytesUntil(interface->modem,',',interBuf,length);
	strncat(buffer,interBuf,bytes);
	
	//LAC
	bytes = mmReadBytesUntil(interface->modem,',',interBuf,length);
	strncat(buffer,interBuf,bytes);

	//CI
	bytes = mmReadBytesUntil(interface->modem,',',interBuf,length);
	strncat(buffer,interBuf,bytes);
	
	return true;
}
