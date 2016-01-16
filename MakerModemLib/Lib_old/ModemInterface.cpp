#include "ModemInterface.h"

ModemInterface::ModemInterface(ModemStream* stream) {
	#ifdef ARDUINO
		this->stream = stream;
	#endif
	
	#ifdef LINUX
		openlog("ModemInterface",0,0);
	#endif
}

ModemInterface::~ModemInterface() {
	#ifdef ARDUINO
		
	#endif
	
	#ifdef LINUX
		closelog();
	#endif
}

//initialization of modem - returns true if all commands completed successfully, false otherwise
bool ModemInterface::init() {
	if (sendCommandCheckForOkay("AT")) {
		int i = 0;				//command counter
		int j = 0;				//retry counter
		while (i < 4 && j < 10) {
			switch(i) {
				case 0:
					(sendCommandCheckForOkay("AT+CMEE=1")) ? i++ : j++;
					break;
				case 1:
					(sendCommandCheckForOkay("AT+UMNOCONF=3,23")) ? i++ : j++;
					break;
				case 2:
					(sendCommandCheckForOkay("AT+CFUN=1")) ? i++ : j++;
					break;
				case 3:
					(sendCommandCheckForOkay("AT+CMGF=1")) ? i++ : j++;
					break;
			}
			#ifdef ARDUINO
				//delay(250);
			#endif
		}
		return (i == 4) ? true : false;
	}
	return false;
}

//Checks for NetworkConnection
bool ModemInterface::checkForConnection() {
	sendCommand("AT+CREG?");
	if (stream->find("0,")) {
		char test = stream->peek();
		if (test == '5' || test == '1'){
			return true;
		}
	}
	return false;
}

//auto checks for OK from data after sending command
bool ModemInterface::sendCommandCheckForOkay(char* cmd) {
	stream->flush();
	sendCommand(cmd);
	sendCommand("\r\n");
	return stream->find("OK");
}

//function for sending commands piecewise
void ModemInterface::sendCommand(char* cmd) {
	#ifdef ARDUINO
		stream->print(cmd);
	#endif
}

//function for sending commands piecewise
void ModemInterface::sendCommand(int cmd) {
	#ifdef ARDUINO
		stream->print(cmd);
	#endif
}

void ModemInterface::flushBuffer() {
	#ifdef ARDUINO
		while (stream->available())
			stream->read();
	#endif
}

//For BeagleBone - priority 0 is Debug, 1 is Info, 2 is Warning, 3 is Error
void ModemInterface::logDebug(char* debug, int priority) {
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
