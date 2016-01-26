#include <iostream>
#include <string>
#include <vector>
#ifdef WIN32
#include <windows.h>
#endif

#include "MakerModem.h"
#include "SMS.h"

#define RECIPIENT_LIMIT 51
#define MESSAGE_LIMIT 161

using namespace std;

void printSMSList(vector<SMS> *);

int main(int argc, char *argv[])
{

	cout << " MakerModem SMS Demo" << endl << endl;
#ifdef WIN32
	MakerModem modem(3);
#endif
#ifdef LINUX
	string device(argv[1]);

	MakerModem modem(device);
#endif
	vector<SMS> * list; 
	SMS * message;
	string buffer;
	string buffer2;
	bool done = false;
	

	if (modem.isConnected())
	{	
		if (modem.init())
		{	
			int count = modem.refreshSMS();
			cout << " There ";
			if (count > 1 || count == 0)
			{
				cout << "are ";
				if (count == 0)
					cout << "no ";
				else
					cout << count;
				cout << "messages" << endl;
			}
			else
				cout << "is 1 message" << endl;

			cout << endl;
		}
	}else
	{
		cout << "Modem could not connect" << endl;
		return 0;
	}

	bool running = true;
	char response;

	while (running)
	{
		cout << " 1 - View SMS list" << endl;
		cout << " 2 - View SMS by ID" << endl;
		cout << " 3 - View SMS by Sender" << endl;
		cout << " 4 - Send SMS" << endl;
		cout << " 5 - Delete SMS by ID" << endl;
		cout << " 6 - End program" << endl;
		cout << endl << " What do you want to do? " ;
		cin >> response;
		cout << endl;

		modem.refreshSMS();

		switch (response)
		{
		case '1':
			list = modem.getSMSList();
			if (list->size() > 0)
				printSMSList(list);
			else
				cout << " No SMS currently" << endl;
			break;
		case '2':
			int id;
			cout << " Please enter ID of SMS: ";
			cin >> id;
			cout << endl;

			message = modem.getSMSObject(id);
			if (message != NULL)
				cout << " " << message->getID() << " : \"" << message->getSender() << "\" : " << message->getMessage() << endl;
			else
				cout << " No SMS matches the ID (" << id << ")" << endl;
			break;
		case '3':
			cout << " Please enter Sender of SMS: ";
			cin >> buffer;
			cout << endl;

			list = new vector<SMS>(modem.getSMSBySender(buffer.c_str()));
			if (list->size() > 0)
				printSMSList(list);
			else
				cout << " No SMS from the Sender (" << buffer << ")" << endl;
			break;
		case '4':
			buffer.clear();
			buffer2.clear();
			cin.ignore();
			cout << " Please enter Recipient (Max 50): ";
			getline(cin, buffer);
			cout << endl << " Please enter message (Max 160): ";
			getline(cin, buffer2);
			cout << endl;

			if (buffer.size() > RECIPIENT_LIMIT)
				buffer.resize(RECIPIENT_LIMIT);

			if (buffer2.size() > MESSAGE_LIMIT)
				buffer2.resize(MESSAGE_LIMIT);

			if (modem.sendSMS(buffer.c_str(), buffer2.c_str()))
				cout << " Message sent" << endl;
			else
				cout << " Unable to send message" << endl;
			break;
		case '5':
			cout << " Please enter ID of SMS: ";
			cin >> id;
			cout << endl;
			message = modem.getSMSObject(id);
			if (message == NULL)
			{
				cout << " No SMS matches the ID (" << id << ")" << endl;
				break;
			}
			done = false;
			while (!done)
			{
				cout << " Are you sure you want to delete? (Y/N) ";
				cin >> response;
				cout << endl;
				switch (response)
				{
				case 'n':
				case 'N':
					done = true;
					break;
				case 'y':
				case 'Y':
					if (modem.deleteSMS(id))
						cout << " Message was deleted" << endl;
					else
						cout << " Unable to delete message" << endl;
					done = true;
					break;
				}
			}
			break;
		case '6':
			running = false;
			break;
		default:
			cout << " Invalid request, please try again" << endl;
		}

		cout << endl << endl;
	}

	cout << " Closing program" << endl;
	return 0;
}


void printSMSList(vector<SMS> * list)
{
	for (std::vector<SMS>::iterator it = list->begin(); it != list->end(); ++it)
		cout << it->getID() << " : \"" << it->getSender() << "\" : " << it->getMessage() << endl;
}
