#include "SMS.h"

using namespace std;

SMS::SMS(int _id, string _status, string _sender, string _date, string _time, string _message)
{
	id = _id;
	status = _status;
	sender = _sender;
	date = _date;
	time = _time;
	message = _message;
}

SMS::~SMS()
{
}