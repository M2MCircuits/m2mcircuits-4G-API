#include "cModemInterface.h"


#ifdef ARDUINO
struct ModemInterface *mmCreate(Stream * device)
{
	struct ModemInterface *iface = (ModemInterface*) malloc(sizeof(struct ModemInterface));
	iface->modem = (ModemStream*) malloc(sizeof(struct ModemStream));
	iface->modem->stream = device;
	return iface;
}
#endif
#ifdef LINUX	
struct ModemInterface *mmCreate(char * blk)
{
	struct ModemInterface *iface = malloc(sizeof(struct ModemInterface)); //new ModemInterface();
	iface->modem = malloc(sizeof(struct ModemStream));
	iface->modem->fd = mmCommInit(blk, 9600);//dummy value if using USB
	iface->modem->peeked = false;
	/*iface->modem->in = fdopen(iface->modem->fd, "r");
	iface->modem->out = fdopen(iface->modem->fd, "w");*/
	return iface;
}
#endif
#ifdef _WIN32
struct ModemInterface *mmCreate(char * blk)
{
	struct ModemInterface *iface = malloc(sizeof(struct ModemInterface)); //new ModemInterface();
	iface->modem = malloc(sizeof(struct ModemStream));
	iface->modem->connected = mmCommInit(blk, iface->modem);//dummy value if using USB
	return iface;
}
#endif

#ifdef LAUNCHPAD
/*
* TODO: Launchpad
*/
#endif


//initialization of modem - returns true if all commands completed successfully, false otherwise
bool mmInit(struct ModemInterface *interf) {
	sleep_seconds(3);
	mmSendCommand(interf, F("+++"));
	sleep_seconds(1);
	interf->dlmode = 0;
	interf->disconnectIndex = 0;
	int i = 0;				//command counter
	mmFlushBuffer(interf);
	for(i = 0; i < 3; i++){
		mmSendCommand(interf, F("AT\r"));
	}
	mmFind(interf->modem, F("OK"));
	mmFlushBuffer(interf);
	i = 0;
	int j = 0;				//retry counter
	while (i < 6 && j < 10) {
		switch(i) {
			case 0:
				(mmSendCommandCheckForOkay(interf, F("ATE1")))?i++:j++; 
				break;
			case 1:
				(mmSendCommandCheckForOkay(interf, F("AT+CMEE=0"))) ? i++ : j++;
				break;
			case 2:
				(mmSendCommandCheckForOkay(interf, F("AT+UMNOCONF=3,23"))) ? i++ : j++;
				break;
			case 3:
				(mmSendCommandCheckForOkay(interf, F("AT+CFUN=1"))) ? i++ : j++;
				break;
			case 4:
				(mmSendCommandCheckForOkay(interf, F("AT+CMGF=1"))) ? i++ : j++;
				break;
			case 5:
				(mmSendCommandCheckForOkay(interf, F("AT+CSDH=1"))) ? i++ : j++; //for text message length
				break;
		}
		#ifdef ARDUINO
			//delay(250);
		#endif
	}
	return (i == 6) ? true : false;
}

//Checks for NetworkConnection
bool mmCheckForConnection(struct ModemInterface *interface) {
	mmSendCommand(interface, F("AT+CREG?\r"));
	if (mmFindChar(interface->modem, '0')) {
		mmRead(interface->modem);
		char test = mmPeek(interface->modem);
		if (test == '5' || test == '1'){
			return true;
		}
	}
	return false;
}

//Checks for NetworkConnection
bool mmCheckForOkay(struct ModemInterface *interface) {
	while (mmFindChar(interface->modem, 'O')) {
		char test = mmPeek(interface->modem);
		if (test == 'K'){
			return true;
		}
	}
	return false;
}


//auto checks for OK from data after sending command
bool mmSendCommandCheckForOkay(struct ModemInterface* interf, char* cmd) {
	mmFlushBuffer(interf);
	mmSendCommand(interf,cmd);
	mmSendCommand(interf,"\r");
	#ifdef ARDUINO
	//mmFind(interf->modem, cmd);
	return mmCheckForOkay(interf);
	#endif
	#ifdef LINUX
	int c;
	mmFind(interf->modem, cmd);
	do{
		c = mmRead(interf->modem);
		if(c < 0){
			usleep(1);
		}else{
			//printf("%c", c);
		}
	} while(c != 'O' && c != 'E');
	return c == 'O';
	#endif
	#ifdef _WIN32
	int c;
	mmFind(interf->modem, cmd);
	do{
		c = mmRead(interf->modem);
		if(c < 0){
			usleep(1);
		}else{
			//printf("%c", c);
		}
	} while(c != 'O' && c != 'E');
	return c == 'O';
	#endif
}

#ifdef ARDUINO
//auto checks for OK from data after sending command
bool mmSendCommandCheckForOkay(struct ModemInterface* interface, FlashStringHelper cmd) {
	mmFlush(interface->modem);
	mmSendCommand(interface,cmd);
	mmSendCommand(interface,"\r");
	mmFind(interface->modem, cmd);
	return mmCheckForOkay(interface);
}
#endif

//function for sending commands piecewise
void mmSendCommand(struct ModemInterface* interf, char* cmd) {
	mmPrint(interf->modem, cmd);
}

#ifdef ARDUINO
//function for sending commands piecewise
void mmSendCommand(struct ModemInterface* interf, FlashStringHelper cmd) {
	mmPrint(interf->modem, cmd);
}
#endif

int mmGetResponse(struct ModemInterface* interf, char* buffer, int length) {
	return mmReadBytes(interf->modem,buffer,length);
}

void mmFlushBuffer(struct ModemInterface* interf) {
	#ifdef ARDUINO
		while (mmAvailable(interf->modem))
			mmRead(interf->modem);
	#endif
	#ifdef LINUX
		while (mmAvailable(interf->modem))
			(mmRead(interf->modem));
		
	#endif
	#ifdef _WIN32
		while (mmAvailable(interf->modem))
			(mmRead(interf->modem));
	#endif
}

//For BeagleBone - priority 0 is Debug, 1 is Info, 2 is Warning, 3 is Error
void mmLogDebug(char* debug, int priority) {
	#ifdef ARDUINO
		//Serial.println(debug);
	#endif
	#ifdef LINUX
	if (priority == 0) {
		syslog (LOG_DEBUG,debug);
	}
	else if (priority == 1) {
		syslog (LOG_INFO,debug);
	}
	else if (priority == 2) {
		syslog (LOG_WARNING,debug);
	}
	else if (priority == 3) {
		syslog (LOG_ERR,debug);
	}
	#endif
}



int testSwig(char *buffer, int length){
	/*printf("%d\n", length);
	for(int i = 0; i < 10; i++){
		buffer[i] = 'a' + i;
	}
	buffer[10] = 0;
	return 10;*/
}
