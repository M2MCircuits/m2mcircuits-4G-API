#include "cSMS.h"

int sendTextMessage(struct ModemInterface* interf, char* number, char* buffer) {
#if defined(LINUX) || defined(ARDUINO)
	mmSendCommand(interf, F("AT+CMGS=\""));
	mmSendCommand(interf, number);
	mmSendCommand(interf, F("\"\r"));
	if (mmFind(interf->modem, F(">"))) {
		mmSendCommand(interf, buffer);
		mmWriteByte(interf->modem, 26);
		mmSendCommand(interf, F("\r"));
		return 1;
}
	return 0;
#endif
#ifdef _WIN32
	char c;
	mmSendCommand(interf, "AT+CMGS=\"");
	mmSendCommand(interf, number);
	mmSendCommand(interf, "\"\r");
	Sleep(1);
	if (mmFind(interf->modem, ">")) {
		mmSendCommand(interf, buffer);
		mmWriteByte(interf->modem, (char)26);
		mmSendCommand(interf, "\r");
		return 1;
	}
	return 0;
#endif
}

int getTextLists(struct ModemInterface* interf, int* ids, int length) {
	int i = 0;
	int temp, j;
#if defined(LINUX) || defined(ARDUINO)
	mmSendCommand(interf, F("AT+CMGL\r"));
	while (mmFind(interf->modem, F("AT+CMGL")) && i < length) {
		ids[i] = mmParseInt(interf->modem);
		i++;
	}
#endif
#ifdef _WIN32
	char * buffer[32];
	char * ptr;
	mmSendCommand(interf, "AT+CMGL\r");
	if (!mmFind(interf->modem, "+CMGL: "))
		return -1;
	else
	{
		if (mmReadBytesUntil(interf->modem, ',', buffer, 32))
		{
			ids[i] = (int)strtol((const char *)buffer, &ptr, 10);
			i++;
		}
	}

	while (mmFind(interf->modem, "+CMGL: ") && i < length) {
		if (mmReadBytesUntil(interf->modem, ',', buffer, 32))
		{
			temp = (int)strtol((const char *)buffer, &ptr, 10);
			for (j = 0; j < i && ids[j] != temp; j++);
			if (j == i)
			{
				ids[i] = temp;
				i++;
			}
		}
	}

	PurgeComm(interf->modem->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
#endif
	return i;
}

int retrieveTextMessage(struct ModemInterface* interf, char* buffer, int length, int id) {
#if defined(LINUX) || defined(ARDUINO)
	mmSendCommand(interf, F("AT+CMGR="))
	mmSendCommand(interf, id);
	mmSendCommand(interf, F("\r"))
	if (mmFind(interf->modem, F("+CMGR: "))) {
		mmFind(interf->modem, F(","));
		mmFind(interf->modem, F(","));
		return mmReadBytesUntil(interf->modem, ',', buffer, length);
	}
	else {
		return 0;
	}
#endif
#ifdef _WIN32
	int total = 0;
	sprintf(buffer, "AT+CMGR=%i", id);
	mmSendCommand(interf, buffer);
	mmSendCommand(interf, "\r");
	if (mmFind(interf->modem, "+CMGR: ")) 
	{
		mmFind(interf->modem, "\n");
		total = mmReadBytesUntil(interf->modem, (char)10, buffer, length);
	}
	PurgeComm(interf->modem->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	return total;
#endif
}

int retrievePhoneNumber(struct ModemInterface* interf, char* buffer, int length, int id) {
#if defined(LINUX) || defined(ARDUINO)
	mmSendCommand(interf, F("AT+CMGR="));
	mmSendCommand(interf, id);
	mmSendCommand(interf, F("\r"));
	if (mmFind(interf->modem, F("+CMGR: "))) {
		mmFind(interf->modem, F(","));
		return mmReadBytesUntil(interf->modem,',',buffer,length);
	}
	else {
		return 0;
	}
#endif
#ifdef _WIN32
	mmSendCommand(interf, "AT+CMGR=");
	mmSendCommand(interf, id);
	mmSendCommand(interf, "\r");
	if (mmFind(interf->modem, "+CMGR: ")) {
		mmFind(interf->modem, ",");
		return mmReadBytesUntil(interf->modem,',',buffer,length);
	}
	else {
		return 0;
	}
#endif
}

int retrieveTimestamp(struct ModemInterface* interf, char* buffer, int length, int id) {
#if defined(LINUX) || defined(ARDUINO)
	mmSendCommand(interf, F("AT+CMGR="));
	mmSendCommand(interf, id);
	mmSendCommand(interf, F("\r"));
	if (mmFind(interf->modem, F("+CMGR: "))) {
		mmFind(interf->modem, F(","));
		mmFind(interf->modem, F(","));
		mmFind(interf->modem, F(","));
		return mmReadBytesUntil(interf->modem,',',buffer,length);
	}
	else {
		return 0;
	}
#endif
#ifdef _WIN32
	mmSendCommand(interf, "AT+CMGR=");
	mmSendCommand(interf, id);
	mmSendCommand(interf, "\r");
	if (mmFind(interf->modem, "+CMGR: ")) {
		mmFind(interf->modem, ",");
		mmFind(interf->modem, ",");
		mmFind(interf->modem, ",");
		return mmReadBytesUntil(interf->modem, ',', buffer, length);
	}
	else {
		return 0;
	}
#endif
}

int deleteTextMessage(struct ModemInterface* interf, int id) {
#if defined(LINUX) || defined(ARDUINO)
	mmSendCommand(interf, F("AT+CMGD="));
	mmSendCommand(interf, id);
	mmSendCommand(interf, F("\r"));
	return mmFind(interf->modem, F("OK"));
#endif
#ifdef _WIN32
	char cmd[128];
	sprintf(cmd, "AT+CMGD=%i", id);
	mmSendCommand(interf, "\r");
	return mmFind(interf->modem, "OK");
#endif
}
