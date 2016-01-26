/* Modem Stream class
 * Handles low-level stream interactions for modem
 * #define [MICROCONTROLLER] before including ModemStream.h
 * Verniel Mesecher, Jagger McCord, Max Roycroft
 */

typedef int bool;
#define true 1
#define false 0

#ifndef MODEMSTREAM
#define MODEMSTREAM

#ifdef ARDUINO
#include <SoftwareSerial.h>
#include <string.h>
#endif

#ifdef LINUX
#include <stdio.h>   //get/put,etc
#include <stdint.h>  
#include <string.h>   
#include <unistd.h>   
#include <fcntl.h>    
#include <errno.h>    
#include <termios.h>  
#include <stdlib.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <termio.h>
#include <err.h>
#include <linux/serial.h>
#include <syslog.h>
#define F(str) str

static int rate_to_constant(int baudrate) {
#define B(x) case x: return B##x
	switch(baudrate) {
		B(50);     B(75);     B(110);    B(134);    B(150);
		B(200);    B(300);    B(600);    B(1200);   B(1800);
		B(2400);   B(4800);   B(9600);   B(19200);  B(38400);
		B(57600);  B(115200); B(230400); B(460800); B(500000); 
		B(576000); B(921600); B(1000000);B(1152000);B(1500000); 
	default: return 0;
	}
#undef B
}    
void sleep_seconds(int seconds);
#endif


#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif




struct ModemStream {
	#ifdef ARDUINO
	Stream* stream;
	#endif
	#ifdef LINUX
	int fd;
	char peekChar;
	bool peeked;
	#endif
	#ifdef WIN32
	HANDLE hSerial; //Serial comm handler
	COMSTAT status; //Get various info about the connection;
	DWORD errors; //keep track of last error
	//LPCWSTR port;
	bool connected;
	char peekChar;
	bool peeked;
	#endif
};

#ifdef ARDUINO
int mmPrint(struct ModemStream* modem, FlashStringHelper buffer);
bool mmFind(struct ModemStream* modem, FlashStringHelper target);
#endif
#ifdef LINUX
int mmCommInit(const char * device, int rate);
#endif
#ifdef _WIN32
int mmCommInit(const char * portname, struct ModemStream* modem);
#endif

int mmAvailable(struct ModemStream* modem);
int mmRead(struct ModemStream* modem);
int mmWrite(struct ModemStream* modem, char *buffer, int bytes);
int mmWriteByte(struct ModemStream* modem, char buffer);
int mmPrint(struct ModemStream* modem, char* buffer);
int mmPrinti(struct ModemStream* modem, int num);
bool mmFlush(struct ModemStream* modem);
bool mmFind(struct ModemStream* modem, char* target);
char mmPeek(struct ModemStream* modem);
int mmReadBytes(struct ModemStream* modem, char* buffer, int length);
int mmReadBytesUntil(struct ModemStream* modem, char target, char* buffer, int length);
long mmParseInt(struct ModemStream* modem);
//bool mmFindUntil(struct ModemStream* modem, char* target, char* term);
#endif