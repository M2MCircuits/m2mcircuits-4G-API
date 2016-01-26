#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

#include "MakerModem.h"
#include "SMS.h"

#define RECIPIENT_LIMIT 51
#define MESSAGE_LIMIT 161

#define BUFSIZE 4096*4 
using namespace std;

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;
void CreateChildProcess(void);
void WriteToPipe(void);
void ReadFromPipe(void);
void ErrorExit(PTSTR);

void printSMSList(vector<SMS> *);

#ifdef UDP_TEST
int main(int argc, char *argv[])
{

	cout << " MakerModem SMS Demo" << endl << endl;

	// SerialComm* SP = new SerialComm("\\\\.\\COM18");
	//SerialComm * SP = new SerialComm("COM3");
	
	//if (SP->IsConnected())
		//cout << "We're connected" << endl ;

	MakerModem modem(3);
	vector<SMS> * list; 
	SMS * message;
	string buffer;
	string buffer2;
	bool done = false;
	

	if (modem.isConnected())
	{
		if (!modem.init())
		{
			cout << "Init failed!" << endl;
			return -1;
			/*int count = modem.refreshSMS();
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

			cout << endl;*/
		}
	}

	modem.attachGPRS();
	for (int i = 0; i < 7; i++) {
		modem.socketClose(i);
	}
	int socketid = -1;// = modem.openTCPSocket();
	//cout << modem.openTCPSocket() << endl;
	cout << socketid << endl;



	SECURITY_ATTRIBUTES saAttr;

	printf("\n->Start of parent execution.\n");

	// Set the bInheritHandle flag so pipe handles are inherited. 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDOUT. 

	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		ErrorExit(TEXT("StdoutRd CreatePipe"));

	// Ensure the read handle to the pipe for STDOUT is not inherited.

	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(TEXT("Stdout SetHandleInformation"));

	// Create a pipe for the child process's STDIN. 

	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
		ErrorExit(TEXT("Stdin CreatePipe"));

	// Ensure the write handle to the pipe for STDIN is not inherited. 

	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(TEXT("Stdin SetHandleInformation"));

	// Create the child process. 

	CreateChildProcess();

	// Get a handle to an input file for the parent. 
	// This example assumes a plain text file and uses string output to verify data flow. 

















	/*modem.tcpConnect(socketid, "google.com", 80);
	modem.tcpWrite(socketid, "GET /\r\n\r\n");
	while (modem.tcpAvailable(socketid) <= 0);
	int avail = modem.tcpAvailable(socketid);
	int cap = avail;
	char *buff = new char[avail + 1];
	buff[avail] = 0;
	int read;
	while (avail > 0) {
		read = modem.tcpRead(socketid, buff+cap-avail, avail);
		avail -= read;
	}
	cout << buff << endl;*/

	if(socketid >= 0)
	modem.socketClose(socketid);
	Sleep(50);
	socketid = modem.socketOpen(false);
	cout << socketid << endl;
	if (socketid < 0) {
		cout << "failed to create socket..." << endl;
		system("pause");
		return -1;
	}
	cout << modem.socketConnect(socketid, "jaggermccord.com", 5846) << endl;
	cout << "Entering dl mode: " << endl;
	if (!modem.enterDirectLink(socketid)) {
		cout << "DL mode failed." << endl;
		return -1;
	}
	else {

	char buf[1025];
		/*while (true) {

			if (modem.directLinkAvailable() > 0) {
				buf[modem.directLinkRead(buf, 1024)] = 0;
				cout << buf;
			}
		}*/

		DWORD dwRead, dwWritten;
		CHAR chBuf[BUFSIZE];
		BOOL bSuccess = FALSE;
		HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

		while (true) {
			bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
			if (bSuccess) {
				modem.directLinkWrite(chBuf, dwRead);
			}
				/*if (modem.directLinkAvailable() > 0) {
					buf[modem.directLinkRead(buf, 1024)] = 0;
					cout << buf;
				}*/
			
		}
		//if (!bSuccess || dwRead == 0) break;

		/*bSuccess = WriteFile(hParentStdOut, chBuf,
			dwRead, &dwWritten, NULL);*/

	}
	return -1;
	int count;
	string in = "";
	/*do {
		modem.directLinkWrite(in.c_str(), in.length());
		while (modem.directLinkAvailable() > 0) {
			//buf[modem.directLinkRead(buf, 1024)] = 0;
			//cout << buf;
		}
		cin >> in;
	} while (in != "QUIT!");*/
	cout << "Exiting..." << endl;;
	modem.exitDirectLink();
	cout << "Exited" << endl;
	cout << modem.sendCommand("AT") << endl;
	system("pause");
	return 0;
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




void WriteToPipe(void)

// Read from a file and write its contents to the pipe for the child's STDIN.
// Stop when there is no more data. 
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;

	for (;;)
	{
		bSuccess = ReadFile(g_hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;

		bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
		if (!bSuccess) break;
	}

	// Close the pipe handle so the child process stops reading. 

	if (!CloseHandle(g_hChildStd_IN_Wr))
		ErrorExit(TEXT("StdInWr CloseHandle"));
}

void ReadFromPipe(void)

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		//if (!bSuccess || dwRead == 0) break;

		bSuccess = WriteFile(hParentStdOut, chBuf,
			dwRead, &dwWritten, NULL);

	for (;;)
	{
		if (!bSuccess) break;
	}
}



void ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
	cout <<endl<<endl<< lpszFunction << endl;
	system("pause");
	exit(-1);
}



void CreateChildProcess()
// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{
	TCHAR szCmdline[] = TEXT("ffmpeg -f gdigrab -framerate 15 -i desktop -vf scale=480:-1 -f mpegts -");
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;

	// Set up members of the PROCESS_INFORMATION structure. 

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 

	bSuccess = CreateProcess(NULL,
		szCmdline,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

					   // If an error occurs, exit the application. 
	if (!bSuccess)
		ErrorExit(TEXT("CreateProcess"));
	else
	{
		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example. 

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
}


#endif