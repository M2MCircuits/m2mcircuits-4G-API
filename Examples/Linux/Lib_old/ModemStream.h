/* Modem Stream class
 * Handles low-level stream interactions for modem
 * #define [MICROCONTROLLER] before including ModemStream.h
 */

#ifndef MODEMSTREAM
#define MODEMSTREAM

#ifdef ARDUINO
#include <SoftwareSerial.h>
#endif

#ifdef LINUX
#include <syslog.h>
#endif

#ifndef CSTRING
#define CSTRING
//#include <cstring>
#endif

class ModemStream 
{

  private:
	#ifdef ARDUINO
		Stream* stream;
	#endif
	#ifdef LINUX
	
	#endif

  public:
	#ifdef ARDUINO
		ModemStream(Stream* stream);
	#endif
	#ifdef LINUX
		ModemStream();
	#endif
    ~ModemStream();
	int available();
	char read();
	int print(char* buffer);
	int print(int num);
	bool flush();
	bool find(char* target);
	bool findUntil(char* target, char* term);
	char peek();
	int readBytes(char* buffer, int length);
	int readBytesUntil(char target, char* buffer, int length);
	//String readString();
	//String readStringUntil(char term);
	long parseInt();
	float parseFloat();
};

#endif
