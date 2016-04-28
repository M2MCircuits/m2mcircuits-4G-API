#include "cInet.h"

bool mmIsGPRSAttached(struct ModemInterface *iface) {
	mmFlushBuffer(iface);
	mmPrint(iface->modem, F("AT+UPSND=0,8\r"));
	mmFind(iface->modem, F("AT+UPSND=0,8\r"));
	if(mmFind(iface->modem, "SND: 0,8,")){
		return mmParseInt(iface->modem);
	}
}
bool mmAttachGPRS(struct ModemInterface *iface){
	if(mmIsGPRSAttached(iface)) return true;
	mmFlushBuffer(iface);
	//load saved profile
	if(!mmSendCommandCheckForOkay(iface, F("AT+UPSDA=0,2"))) return false;
	//activate
	return mmSendCommandCheckForOkay(iface, F("AT+UPSDA=0,3"));
	
}
bool mmInitGPRS(struct ModemInterface *iface){
	//create and save profile
	if(!mmSendCommandCheckForOkay(iface, F("AT+UPSD=0,100,8"))) return false;
	if(!mmSendCommandCheckForOkay(iface, F("AT+UPSD=0,0,2"))) return false;
	if(!mmSendCommandCheckForOkay(iface, F("AT+UPSDA=0,1"))) return false;
	return true;
}
int mmSocketOpen(struct ModemInterface *iface, bool tcp){
	mmFlushBuffer(iface);
	mmPrint(iface->modem, F("AT+USOCR="));
	if(tcp){
		mmPrinti(iface->modem, 6);
	}else{
		mmPrinti(iface->modem, 17);
	}
	mmPrint(iface->modem, "\r");
	mmFind(iface->modem, F("AT+USOCR="));
	mmFind(iface->modem, "\r");
	//mmFlush(iface->modem);
	//printf("HERE!!");
	int c;
	do{
		c = mmRead(iface->modem);
		
	}while(c != '+' && c != 'E');
	c = mmRead(iface->modem);
	if(c != 'U') return -1;
	putchar('\n');
	mmFind(iface->modem, "SOCR: ");
	//printf("HERE!!");
	return mmParseInt(iface->modem);
}

bool mmSocketConnect(struct ModemInterface *iface, int sid, char *dest, int port){
	mmFlushBuffer(iface);
	mmPrint(iface->modem, F("AT+USOCO="));
	mmPrinti(iface->modem, sid);
	mmPrint(iface->modem, ",\"");
	mmPrint(iface->modem, dest);
	mmPrint(iface->modem, "\",");
	mmPrinti(iface->modem, port);
	mmPrint(iface->modem, "\r");
	mmFind(iface->modem, F("AT+USOCO="));
	mmFind(iface->modem, "\r");
	int c;
	do{
		c = mmRead(iface->modem); 
		//putchar(c);
	}while(c!= 'E' && c != '+' && c != 'O');
	if(c == 'O') return true;
	return false;
}
int mmSocketAvail(struct ModemInterface *iface, int sid){
	mmFlushBuffer(iface);
	mmPrint(iface->modem, F("AT+USORD="));
	mmPrinti(iface->modem, sid);
	mmPrint(iface->modem, ",0\r");
	mmFind(iface->modem, F("AT+USORD="));
	mmFind(iface->modem, "\r");
	int c;
	do {
		c = mmRead(iface->modem);
	} while (c != '+' && c != 'E');
	c = mmRead(iface->modem);
	if( c != 'U') {
		return -1; //error
	}
	for(int i = 0; i < 8; i++){ //skip past SORD: #,
		if(mmRead(iface->modem) < 0){
			i--;
			continue;
		}
	}
	return mmParseInt(iface->modem);
}
void mmSocketClose(struct ModemInterface *iface, int sid){
	mmFlushBuffer(iface);
	mmPrint(iface->modem, F("AT+USOCL="));
	mmPrinti(iface->modem, sid);
	mmPrint(iface->modem, "\r");
	mmFind(iface->modem, F("AT+USOCL="));
	mmFind(iface->modem, "\r");
}

int mmSocketWriteBytes(struct ModemInterface *iface, int sid, char *buff, int len){
	if(len > 1024) len = 1024; //can only send a max of 1024 bytes at a time.
	mmFlushBuffer(iface);
	mmPrint(iface->modem, F("AT+USOWR="));
	mmPrinti(iface->modem, sid);
	mmPrint(iface->modem, ",");
	mmPrinti(iface->modem, len);
	mmPrint(iface->modem, "\r");
	mmFind(iface->modem, F("AT+USOWR="));
	int c;
	do {
		c = mmRead(iface->modem);
	}while(c != '@' && c != 'E') ;
	if(c == 'E') return -1;
	int tmp;
	int written = len;
	while(len > 0) {//ensure everything gets written
		tmp = mmWrite(iface->modem, buff, len);
		buff+=tmp;
		len-=tmp;
	}
}
int mmSocketWrite(struct ModemInterface *iface, int sid, char *buff){
	mmSocketWriteBytes(iface, sid, buff, strlen(buff));
}
int mmSocketRead(struct ModemInterface *iface, int sid, char *buff, int maxlen){
	if(maxlen > 1024) maxlen = 1024; //same as with write
	if(maxlen <= 0) return 0;
	mmFlushBuffer(iface);
	mmPrint(iface->modem, F("AT+USORD="));
	mmPrinti(iface->modem, sid);
	mmPrint(iface->modem, ",");
	mmPrinti(iface->modem, maxlen);
	mmPrint(iface->modem, "\r");
	mmFind(iface->modem, F("AT+USORD="));
	
	int c;
	do{
		c = mmRead(iface->modem);
		
	}while(c != '+' && c != 'E');
	if(c == 'E') return 0;
	mmFind(iface->modem, "USORD: ");
	mmFind(iface->modem, ",");
	int bytesRead = mmParseInt(iface->modem);
	printf("read: %i\n", bytesRead);
	if(bytesRead < maxlen) maxlen = bytesRead;
	mmFind(iface->modem, "\"");
	int tmp;
	while(maxlen > 0) { //ensure we get all our bytes
		tmp = mmReadBytes(iface->modem, buff, maxlen);
		buff+=tmp;
		maxlen-=tmp;
	}
	return bytesRead;
}

bool mmEnterDirectLink(struct ModemInterface *iface, int sid){
	if(iface->dlmode) return -1;
	mmFlushBuffer(iface);
	mmPrint(iface->modem, "AT+USODL=");
	mmPrinti(iface->modem, sid);
	mmPrint(iface->modem, "\r");
	mmFind(iface->modem, "AT+USODL=");
	int c;
	do{
		c = mmRead(iface->modem);
		
	}while(c != 'C' && c != 'E');
	if(c == 'E') return 0; //aww, an error
	iface->dlmode = 1;
	mmFind(iface->modem, "ONNECT");
	mmFlushBuffer(iface);
	return 1;
}
//note, only valid AFTER initialization.  
bool mmCheckDirectLink(struct ModemInterface *iface){
	return iface->dlmode;
}
bool mmExitDirectLink(struct ModemInterface *iface){
	mmFlushBuffer(iface);
	sleep_seconds(2);
	mmWrite(iface->modem, "+++", 3);
	sleep_seconds(1);
	iface->dlmode = 0;
	mmFind(iface->modem, "DISCONNECT");
	return 1; //doubt this can fail.
}
int mmDirectLinkWrite(struct ModemInterface *iface, char *buff, int count){
	if(!iface->dlmode) return -1; //error
	return mmWrite(iface->modem, buff, count);
}
int mmDirectLinkRead(struct ModemInterface *iface, char *buff, int count){
	static const char disconnectBuffer[10] = {'D','I','S','C','O','N','N','E','C','T'}; //annoying to type out but saves a byte..
	if(!iface->dlmode) return -1; //error
	int read = mmReadBytes(iface->modem, buff, count);
	//since there are no prefixes, we don't need a full KMP here
	for(int i = 0; i < read; i++){
		if(disconnectBuffer[iface->disconnectIndex] == buff[i]) iface->disconnectIndex++;
		else{
			iface->disconnectIndex = buff[i] == 'D'; //if no match, check if it's the first character
		}
		if(iface->disconnectIndex == 10){
			iface->dlmode = 0; 
			read -= 10;
			if(read < 0) read = 0;
			buff[read] = 0;
			break;
		}
	}
	
	return read;
	
}

int mmDirectLinkAvailable(struct ModemInterface *iface){
	return mmAvailable(iface->modem);
}
