#include "SerialComm.h"

#ifdef WIN32
SerialComm::SerialComm(const char *portName)
{
	//We're not yet connected
	this->connected = false;

	std::string filepath(portName);
	std::wstring stemp = s2ws(filepath);
	LPCWSTR result = stemp.c_str();

	//Try to connect to the given port throuh CreateFile
	this->hSerial = CreateFileW(result,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Check if the connection was successfull
	if (this->hSerial == INVALID_HANDLE_VALUE)
	{
		//If not success full display an Error
		if (GetLastError() == ERROR_FILE_NOT_FOUND){

			//Print Error if neccessary
			printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			//If impossible, show an error
			printf("failed to get current serial parameters!");
		}
		else
		{
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = CBR_9600;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			//Setting the DTR to Control_Enable ensures that the Arduino is properly
			//reset upon establishing a connection
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				printf("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				//If everything went fine we're connected
				this->connected = true;
				//Flush any remaining characters in the buffers 
				PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				//We wait 2s as the arduino board will be reseting
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}
}
#endif
#ifdef LINUX
SerialComm::SerialComm(std::string device)
{
	fd = comm_init(device.c_str(), baudrate);
	
	if (fd == -1)
	{
		connected = false;
		//std::cout << "Unable to connect" << std::endl;
	}else
		connected = true;
}

/* Open serial port in raw mode, with custom baudrate if necessary */
int SerialComm::comm_init(const char *device, int rate)
{
	struct termios options;
	struct serial_struct serinfo;
	int fd;
	int speed = 0;

	/* Open and configure serial port */
	if ((fd = open(device,O_RDWR|O_NOCTTY|O_NONBLOCK)) == -1)
		return -1;
	
	speed = rate_to_constant(rate);

	if (speed == 0) {
		/* Custom divisor */
		serinfo.reserved_char[0] = 0;
		if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
			return -1;
		serinfo.flags &= ~ASYNC_SPD_MASK;
		serinfo.flags |= ASYNC_SPD_CUST;
		serinfo.custom_divisor = (serinfo.baud_base + (rate / 2)) / rate;
		if (serinfo.custom_divisor < 1) 
			serinfo.custom_divisor = 1;
		if (ioctl(fd, TIOCSSERIAL, &serinfo) < 0)
			return -1;
		if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
			return -1;
		if (serinfo.custom_divisor * rate != serinfo.baud_base) {
			warnx("actual baudrate is %d / %d = %f",
			      serinfo.baud_base, serinfo.custom_divisor,
			      (float)serinfo.baud_base / serinfo.custom_divisor);
		}
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);
	tcgetattr(fd, &options);
	cfsetispeed(&options, speed ?: B38400);
	cfsetospeed(&options, speed ?: B38400);
	cfmakeraw(&options);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~CRTSCTS;
	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 5;
	if (tcsetattr(fd, TCSANOW, &options) != 0)
		return -1;
	
	return fd;
}

#endif

SerialComm::~SerialComm()
{
	if (this->connected)
	{
		this->connected = false;
		#ifdef WIN32
		CloseHandle(this->hSerial);
		#endif
		#ifdef LINUX
		close(fd);
		#endif
		//cout << "Comm closed" << endl;
	}
}
int SerialComm::readData(char* buffer, unsigned int length)
{
	
	#ifdef LINUX
	tcdrain(fd);
	//fcntl(fd, F_SETFL, O_NONBLOCK);
	
	int n = read(fd, buffer, length);

	return n;
	#endif
	#ifdef WIN32
	
	DWORD bytesRead;
	unsigned int toRead;

	ClearCommError(this->hSerial, &this->errors, &this->status);

	//Check if there is something to read
	if (this->status.cbInQue > 0)
	{
		//If there is we check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if (this->status.cbInQue > length)
		{
			toRead = length;
		}
		else
		{
			toRead = this->status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) && bytesRead != 0)
		{
			return bytesRead;
		}

	}

	//If nothing has been read, or that an error was detected return -1
	return -1;
	#endif
}

int SerialComm::writeData(const char * buffer, unsigned int nbChar){
	if(this->connected){
		#ifdef WIN32		
		DWORD bytesSend;
		//Try to write the buffer on the Serial port
		if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
		{
			//In case it don't work get comm error and return false
			ClearCommError(this->hSerial, &this->errors, &this->status);
	
			return -1;
		}
		else
			return bytesSend;
		#endif
		#ifdef LINUX
		return write(fd, buffer, nbChar);
		#endif
	}
	return -1;
}
int SerialComm::writeData(std::string message)
{
	if(this->connected)
	{
		int length = message.length();
		char * cstr = new char[length + 3];
	
		strcpy(cstr, message.c_str());
		cstr[length] = '\r';
		cstr[length + 1] = '\n';
		cstr[length + 2] = '\0';
		
		int n = writeData((const char *)cstr, length + 3);
		if (n != length + 3)
			return -1;
		return n;
	}
	return -1;
}

bool SerialComm::IsConnected()
{
	return this->connected;
}
