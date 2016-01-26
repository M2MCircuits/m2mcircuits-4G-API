#include "MakerModem.h"

#ifdef WIN32

MakerModem::MakerModem(int comm_port)
{
	//set up serial communication
	//string connection = "\\\\.\\COM" + comm_port;
	string connection("COM");
	connection += to_string(comm_port);
	SP = new SerialComm(connection.c_str());
}
#endif
#ifdef LINUX
MakerModem::MakerModem(string device)
{
	SP = new SerialComm(device);
}
#endif


MakerModem::~MakerModem()
{
	//delete[] SP;
}


bool MakerModem::init()
{
	
	//setup network settings
	const char * init_commands[4] = {
		"AT+CMEE=1",
		"AT+UMNOCONF=3,23",
		"AT+CFUN=1",
		"AT+CMGF=1"
	};

	if (SP->IsConnected())
	{
		int index = 0;
		while (index < 4)
		{
			
			string message(init_commands[index]);
			string response = sendCommand(message.c_str());
			
			if (strncmp(response.c_str(), "OK", 2) == 0)
				index++;

			Sleep(1);
		}
	}
	else
		return false;
	
	return true;
}

string MakerModem::sendCommand(const char * command)
{
	string response("");
	memset(buffer, 0, BUFFER_SIZE);

	if (SP->IsConnected() && SP->writeData(command)!= -1)
	{
		bool finished = false;
		while (!finished)
		{
			Sleep(1);			//wait for response
			if (SP->readData(buffer, BUFFER_SIZE) != -1)
			{
				char * position;
				if ((position = strstr(buffer, command)) != NULL)			//contains command
				{
					finished = true;
					//remove everything before the command response
					response = position;
					//remove command from response
					response.erase(0, strlen(command));

					//remove newlines and carriage returns from beginning
					while (response.size() > 0 && (response.at(0) == '\n' || response.at(0) == '\r'))
						response.erase(0, 1);

					//remove newlines and carriage returns from end
					while (response.size() > 0 && (response.at(response.size() - 1) == '\n' || response.at(response.size() - 1) == '\r'))
						response.erase(response.size() - 1);
				}
			}
			else
			{
				finished = true;
			}
		}
	}
	return response;
}

int MakerModem::refreshSMS()
{
	messages.clear();								//maybe replace with second buffer to prevent data loss, match against older version
	string command("AT+CMGL");
	string test("+CMGL: ");
	int count = 0;

	string response = sendCommand(command.c_str());

	//check if string begins with "OK"
	if (strncmp(response.c_str(), "OK", 2) != 0)
	{
		//messages exist
		string line, sender, date, time, status, message;
		stringstream ss(response);
		int id;
		bool isMessage = false;

		while (getline(ss, line))
		{
			if (line[0] != '\r' && line.compare(0, 2, "OK") != 0)
			{
				if (line.compare(0, test.length(), test) == 0)				//message info
				{
					isMessage = true;
					string tokens[5];
					int index = 0;

					line.erase(0, test.size());

					//tokenize line
					char s[1024];
					strcpy(s, line.c_str());
					char * token = strtok(s, ",");
					while (token)
					{
						tokens[index++] = token;
						token = strtok(NULL, ",");
					}

					//get id
					id = atoi(tokens[0].c_str());

					//get message status
					tokens[1].erase(0, 1);
					tokens[1].erase(tokens[1].size() - 1);
					status = tokens[1];

					//get sender
					tokens[2].erase(0, 1);
					tokens[2].erase(tokens[2].size() - 1);
					sender = tokens[2];

					//get date
					tokens[3].erase(0, 1);
					date = tokens[3];											//digits for each are reversed

					//get time
					tokens[4].erase(tokens[4].size() - 1);
					tokens[4].erase(tokens[4].size() - 1);						//digits for each are reversed
					time = tokens[4];
				}
				else														//message
				{
					if (isMessage)
					{
						//get message
						message = line;
						message.erase(message.size() - 1);
						SMS item(id, status, sender, date, time, message);
						messages.push_back(item);
						count++;
						isMessage = false;
					}
				}
			}
		}
	}
	return count;
}

bool MakerModem::deleteSMS(int id)
{
	bool success = false;
	string command("AT+CMGD=");
	command.append(to_string(id));

	string response = sendCommand(command.c_str());

	if (strstr(response.c_str(), "OK") != NULL)
	{
		success = true;
		messages.erase(messages.begin() + id);
	}

	return success;
}

bool MakerModem::sendSMS(const char * recipient, const char * message)
{
	bool success = false;
	string command("AT+CMGS=");
	command.append(recipient);

	string response = sendCommand(command.c_str());

	if (response[0] == '>')
	{
		success = true;
		command = message;
		command += (char)CTRL_Z;
		response = sendCommand(command.c_str());
		if (strstr(response.c_str(), "OK") != NULL)
			success = true;
	}
	return success;
}


string MakerModem::getSMS(int id)
{
	string response = "";
	for (std::vector<SMS>::iterator it = messages.begin(); it != messages.end(); ++it) {
		if (it->getID() == id)
		{
			response = it->getMessage();
			break;
		}
	}
	return response;
}

SMS * MakerModem::getSMSObject(int id)
{
	for (std::vector<SMS>::iterator it = messages.begin(); it != messages.end(); ++it) {
		if (it->getID() == id)
			return &(*it);
	}
	return NULL;
}

vector<SMS> MakerModem::getSMSBySender(const char * number)
{
	vector<SMS> list;
	string cleaned(clean_number(number));
	for (std::vector<SMS>::iterator it = messages.begin(); it != messages.end(); ++it) {
		string current = it->getSender();
		if (strcmp(cleaned.c_str(), clean_number(current.c_str()).c_str()) == 0)
			list.push_back(*it);
	}
	return list;
}

string MakerModem::clean_number(const char * number)
{
	int i;
	int length = strlen(number);
	string response = "";

	for (i = 0; i < length; i++)
	{
		if ('0' <= number[i] && number[i] <= '9')
			response += number[i];
		if (number[i] == 0)
			break;
	}

	return response;
}
