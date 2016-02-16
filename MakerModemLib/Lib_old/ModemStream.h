/*
    This file is part of the MakerModem API.

    The MakerModem API is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The MakerModem API is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the MakerModem API.  If not, see <http://www.gnu.org/licenses/>.
*/

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
