#pragma once
#include <iostream>
#include <string>

class SMS
{
	int id;
	std::string status;
	std::string message;
	std::string sender;
	std::string date;
	std::string time;

public:
	SMS(int, std::string, std::string, std::string, std::string, std::string);
	~SMS();

	int getID() {return id;}
	const char * getMessage() {return message.c_str();}
	const char * getSender() { return sender.c_str(); }
};

