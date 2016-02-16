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

#pragma once
#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H
#ifdef LINUX
#include <stdio.h>    /* Standard input/output definitions */
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>
#include <termio.h>
#include <err.h>
#include <linux/serial.h>
#include <iostream>
#endif

#include <string>
#include <stdlib.h>

#define ARDUINO_WAIT_TIME 20
#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif
#ifdef LINUX
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
#endif
class SerialComm
{
	private:
#ifdef LINUX
	int fd;
	char serialport[256], portname[128];
	const int baudrate=9600; //115200;  // default
	int rc, n, i; 
	unsigned char byte;
	unsigned int data;
	
#endif
	bool connected;
#ifdef WIN32
	HANDLE hSerial; //Serial comm handler
	COMSTAT status; //Get various info about the connection;
	DWORD errors; //keep track of last error
#endif
	int comm_init(const char*, int);

	//Writes data from a buffer through the Serial connection
	//return true on success.
	int writeData(char *buffer, unsigned int nbChar);
	
public:
	
#ifdef LINUX
	SerialComm(std::string device);
#endif
#ifdef WIN32
	SerialComm(const char *portname);
#endif
	~SerialComm();
	//Check if we are actually connected
	bool IsConnected();
	
	int readData(char *buffer, unsigned int nbChar);
	int writeData(std::string message);
	int writeData(const char *buffer, unsigned int nbChar);
};

#ifdef WIN32
	static std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}
#endif

#endif
