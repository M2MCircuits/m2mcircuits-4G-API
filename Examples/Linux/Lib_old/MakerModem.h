#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#ifdef LINUX
#include <unistd.h>
#include <cstring>
#define Sleep(x) usleep(1000*x)
#endif

#include "SerialComm.h"
#include "SMS.h"

#define BUFFER_SIZE 4096
#define CTRL_Z 26

using namespace std;


class MakerModem
{
	//serial communication object
	SerialComm * SP;
	char buffer[BUFFER_SIZE];
	int length;
	int readResult;

	//SMS
	vector<SMS> messages;

	string clean_number(const char * number);

public:
#ifdef WIN32
	MakerModem(int comm_port);
#endif
#ifdef LINUX
	MakerModem(string device);
#endif
	~MakerModem();

	bool init();
	bool isConnected()
	{
		return SP->IsConnected();
	};

	string sendCommand(const char * command);

	int refreshSMS();
	bool deleteSMS(int);
	bool sendSMS(const char * recipient, const char * message);
	string getSMS(int);
	SMS * getSMSObject(int);
	vector<SMS> getSMSBySender(const char *);
	vector<SMS> * getSMSList() { return &messages; };



	bool isGPRSAttached();
	bool attachGPRS();
	int socketOpen(bool tcp);
	bool socketConnect(int sid, const char *dest, int port);
	int socketAvail(int sid);
	void socketClose(int sid);
	
	int socketWrite(int sid, const char *buff, int len);
	int socketWrite(int sid, const char *buff) { return socketWrite(sid, buff, strlen(buff)); }
	int socketWrite(int sid, const char buff) { return socketWrite(sid, &buff, 1); }
	int socketRead(int sid, char *buff, int len);
	int udpWrite(int sid, const char *dest, int port, const char *buff, int len);
	int udpWrite(int sid, const char *dest, int port, const char *buff) { return udpWrite(sid, dest, port, buff, strlen(buff)); }
	int udpWrite(int sid, const char *dest, int port, const char buff) { return udpWrite(sid, dest, port, &buff, 1); }

	bool enterDirectLink(int);
	bool exitDirectLink();
	//int directLinkAvailable();
	int directLinkRead(char * buff, int count);
	int directLinkWrite(const char * buff, int count);

};

