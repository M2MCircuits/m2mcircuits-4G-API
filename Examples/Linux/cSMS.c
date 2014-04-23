#include "cSMS.h"

bool mmSendTextMessage(struct ModemInterface* interface, char* number, char* buffer) {
	mmSendCommand(interface, F("AT+CMGS=\""));
	mmSendCommand(interface, number);
	mmSendCommand(interface, F("\"\r"));
	mmFind(interface->modem, F("AT+CMGS=\""));
	
	int c = mmFindChars(interface->modem, 'E', '>');
	//sleep_millis(milliTimeoutSMS);
	if (c == '>') {
		mmSendCommand(interface, buffer);
		mmWriteByte(interface->modem,26);
		mmSendCommand(interface, F("\r"));
		return true;
	}
	return false;
}

int mmGetTextLists(struct ModemInterface* interface, int* ids, int length) {
	mmSendCommand(interface, F("AT+CMGL\r"));
	mmFind(interface->modem, F("AT+CMGL\r"));
	int count = 0;
	int c;
	int len;
	while(count < length){
	int c = mmFindChars(interface->modem, 'O', '+');
		if(c == '+'){
			mmFindChar(interface->modem, ' '); //
			ids[count++] = mmParseInt(interface->modem);
			mmFindChar(interface->modem, ':');
			mmFindChar(interface->modem, ',');
			mmFindChar(interface->modem, ',');
			len = mmParseInt(interface->modem);
			mmFindChar(interface->modem, '\r');
			for(int i = 0; i < len; i++) if(mmRead(interface->modem) < 0) i--;
		}else{
			break;
		}
	}
	return count;
	
	/*while (mmFindChar(interface->modem, '+') && i < length) {
		ids[i] = mmParseInt(interface->modem);
		i++;
		//mmFindChar(interface->modem, '+');
	}
	return i;*/
}

int mmRetrieveTextMessage(struct ModemInterface* interface, char* buffer, int length, int id) {
	mmSendCommand(interface, F("AT+CMGR="));
	mmPrinti(interface->modem, id);
	//mmSendCommand(interface, id);
	mmWriteByte(interface->modem, '\r');
	mmFind(interface->modem, F("AT+CMGR="));
	int c = mmFindChars(interface->modem, 'E', ':');
	int len;
	if (c == ':') { //mmFindChar(interface->modem, ':')) {
		mmFindChar(interface->modem, ':');
		for(int i = 0; i < 7; i++) mmFindChar(interface->modem, ',');
		
		len = mmParseInt(interface->modem);
		mmFindChar(interface->modem, '\n');
		if(length > len) length = len;
		len = length;
		int count;
		do{
			count = mmReadBytes(interface->modem,buffer,length);
			buffer+=count;
			length-=count;
		}while(length > 0);
		buffer[0] = 0;
		return len;
	}
	else {
		return 0;
	}
}

bool mmRetrievePhoneNumber(struct ModemInterface* interface, char* buffer, int length, int id) {
	mmSendCommand(interface, F("AT+CMGR="));
	mmPrinti(interface->modem, id);
	//mmSendCommand(interface, id);
	mmWriteByte(interface->modem, '\r');
	mmFind(interface->modem, F("AT+CMGR="));
	int c = mmFindChars(interface->modem, 'E', ':');
	if (c == ':') {
		mmFindChar(interface->modem, '+');
		int index = 0;
		length--; //save room for null terminator
		while(index < length){
			c = mmRead(interface->modem);
			if( c < 0) continue;
			if(c == '\"') break;
			buffer[index++] = c;
		}
		buffer[index] = 0;
		return index;
	}
	else {
		return 0;
	}
}

bool mmRetrieveTimestamp(struct ModemInterface* interface, char* buffer, int length, int id) {
	mmSendCommand(interface, F("AT+CMGR="));
	mmPrinti(interface->modem, id);
	//mmSendCommand(interface, id);
	mmSendCommand(interface, F("\r"));
	mmFind(interface->modem, F("AT+CMGR="));
	int c = mmFindChars(interface->modem, 'E', ':');
	if (c == ':') {
		mmFindChar(interface->modem, ',');
		mmFindChar(interface->modem, ',');
		mmFindChar(interface->modem, ',');
		mmFindChar(interface->modem, ',');
		mmFindChar(interface->modem, '"');
		return mmReadBytesUntil(interface->modem,'"',buffer,length);
	}
	else {
		return 0;
	}
}

bool mmDeleteTextMessage(struct ModemInterface* interface, int id) {
	mmSendCommand(interface, F("AT+CMGD="));
	mmPrinti(interface->modem, id);
	//mmSendCommand(interface, id);
	mmSendCommand(interface, F("\r"));
	mmFind(interface->modem, F("AT+CMGD="));
	return mmCheckForOkay(interface);
}
