#include "cModemInterface.h"


#ifdef ARDUINO
struct ModemInterface *mmCreate(Stream * device)
{
	struct ModemInterface *iface = new ModemInterface();
	iface->modem = new ModemStream();
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


//initialization of modem - returns true if all commands completed successfully, false otherwise
int mmInit(struct ModemInterface *interf) {
	int i = 0;				//command counter
	int j = 0;				//retry counter
	
	#ifdef LINUX
	sleep_seconds(3);
	mmSendCommand(interf, F("+++"));
	sleep_seconds(1);
	interf->dlmode = 0;
	interf->disconnectIndex = 0;
	mmFlushBuffer(interf);
	#endif
	#ifdef ARDUINO
	delay(3000);
	mmSendCommand(interf, F("+++"));
	delay(1000);
	interf->dlmode = 0;
	interf->disconnectIndex = 0;
	mmFlushBuffer(interf);
	#endif
	#ifdef _WIN32
	Sleep(300);
	mmSendCommand(interf, "+++");
	Sleep(100);
	interf->dlmode = 0;
	interf->disconnectIndex = 0;
	//mmFlushBuffer(interf);
	#endif

#if defined(LINUX) || defined(ARDUINO)
	for(i = 0; i < 3; i++){
		mmSendCommand(interf, F("AT\r"));
	}
	mmFind(interf->modem, F("OK"));
	mmFlushBuffer(interf);
#endif
#ifdef _WIN32
	mmSendCommand(interf, "AT\r");
	mmFind(interf->modem, "OK");
	//mmFlush(interf->modem);
#endif
	i = 0;
	while (i < 5 && j < 10) {
		switch(i) {
			case 0:
#if defined(LINUX) || defined(ARDUINO)
				(mmSendCommandCheckForOkay(interf, F("ATE1")))?i++:j++; 
#endif
#ifdef _WIN32
				(mmSendCommandCheckForOkay(interf, "ATE1")) ? i++ : j++;
#endif
				break;
			case 1:
#if defined(LINUX) || defined(ARDUINO)
				(mmSendCommandCheckForOkay(interf, F("AT+CMEE=0"))) ? i++ : j++;
#endif
#ifdef _WIN32
				(mmSendCommandCheckForOkay(interf, "AT+CMEE=0")) ? i++ : j++;
#endif
				break;
			case 2:
#if defined(LINUX) || defined(ARDUINO)
				(mmSendCommandCheckForOkay(interf, F("AT+UMNOCONF=3,23"))) ? i++ : j++;
#endif
#ifdef _WIN32
				(mmSendCommandCheckForOkay(interf, "AT+UMNOCONF=3,23")) ? i++ : j++;
#endif
				break;
			case 3:
#if defined(LINUX) || defined(ARDUINO)
				(mmSendCommandCheckForOkay(interf, F("AT+CFUN=1"))) ? i++ : j++;
#endif
#ifdef _WIN32
				(mmSendCommandCheckForOkay(interf, "AT+CFUN=1")) ? i++ : j++;
#endif	
				break;
			case 4:
#if defined(LINUX) || defined(ARDUINO)
				(mmSendCommandCheckForOkay(interf, F("AT+CMGF=1"))) ? i++ : j++;
#endif
#ifdef _WIN32
				(mmSendCommandCheckForOkay(interf, "AT+CMGF=1")) ? i++ : j++;
#endif
				
				break;
		}
		#ifdef ARDUINO
			//delay(250);
		#endif
	}
	mmFlush(interf->modem);
	return (i == 5) ? 1 : 0;
}

//Checks for NetworkConnection
int mmCheckForConnection(struct ModemInterface *interf) {
#if defined(LINUX) || defined(ARDUINO)
	mmSendCommand(interf, F("AT+CREG?"));
#endif
#ifdef _WIN32
	bool done = false;
	int result = 0;
	while (!done)
	{
		mmSendCommand(interf, "AT+CREG?");
#endif
		if (mmFind(interf->modem, "0,")) {
			char test = mmPeek(interf->modem);
			if (test == '5' || test == '1'){
				result = 1;
				done = true;
			}
			else
			{
				result = 0;
				done = true;
			}
		}
#ifdef _WIN32
	}
#endif
	return 0;
}

//auto checks for OK from data after sending command
int mmSendCommandCheckForOkay(struct ModemInterface* interf, char* cmd) {
	#ifdef ARDUINO
	mmFlushBuffer(interf);
	mmSendCommand(interf,cmd);
	mmSendCommand(interf,"\r");
	mmFind(interf->modem, cmd);
	return mmFind(interf->modem, F("OK"));
	#endif
	#ifdef LINUX
	mmFlushBuffer(interf);
	mmSendCommand(interf,cmd);
	mmSendCommand(interf,"\r");
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
	char c;
	PurgeComm(interf->modem->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	mmSendCommand(interf, cmd);
	mmSendCommand(interf, "\r");
	mmFind(interf->modem, cmd);
	do{
		c = (char)mmRead(interf->modem);
		if(c < 0){
			Sleep(1);
		}
	} while(c != 'O' && c != 'E');
	return c == 'O';
#endif
}

#ifdef ARDUINO
//auto checks for OK from data after sending command
int mmSendCommandCheckForOkay(struct ModemInterface* interf, FlashStringHelper cmd) {
	mmFlush(interf->modem);
	mmSendCommand(interf,cmd);
	mmSendCommand(interf,"\r");
	mmFind(interf->modem, cmd);
	return mmFind(interf->modem, F("OK"));
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
		//mmFlush(interf->modem);
		/*printf("%i\n", mmAvailable(interf->modem));*/
		//printf("Begin flush:\n");
		while (mmAvailable(interf->modem))
			(mmRead(interf->modem));
		//printf("\nend flush: \n");
		
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
