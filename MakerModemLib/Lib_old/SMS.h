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

