/* Abstraction class for the ModemStream
 * General stream functions used in ModemInterface class
 * Uses ifdefs to cut down on program length
 */
 
#include "cModemStream.h"

#ifdef ARDUINO
void sleep_seconds(int seconds){
	delay(seconds*1000);
}
void sleep_millis(int millis){
	delay(millis);
}
#endif

#ifdef LINUX
void sleep_seconds(int seconds){
	while(seconds= sleep(seconds));
}
void sleep_millis(int millis){
	usleep(millis*1000);
}


/* Open serial port in raw mode, with custom baudrate if necessary */
int mmCommInit(const char *device, int rate)
{
	struct termios options;
	struct serial_struct serinfo;
	int fd;
	int speed = 0;

	/* Open and configure serial port */
	if ((fd = open(device,O_RDWR|O_NOCTTY/*|O_NONBLOCK*/)) == -1)
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

	//fcntl(fd, F_SETFL, O_NONBLOCK);
	tcgetattr(fd, &options);
	cfsetispeed(&options, speed ?: B38400);
	cfsetospeed(&options, speed ?: B38400);
	cfmakeraw(&options);
	options.c_lflag  &= ~(ICANON|ECHO);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~CRTSCTS;
	options.c_cc[VMIN] = 0;
	//options.c_cc[VTIME] = 200;
	if (tcsetattr(fd, TCSANOW, &options) != 0)
		return -1;
	
	tcflush(fd, TCIFLUSH);
	return fd;
}

#endif

#ifdef _WIN32
int mmCommInit(const char *portname, struct ModemStream* modem)
{
	//We're not yet connected
	int connected = 0;

	
	LPCWSTR result = (LPCWSTR) portname;

	//Try to connect to the given port throuh CreateFile
	modem->hSerial = CreateFileW(result,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Check if the connection was successfull
	if (modem->hSerial == INVALID_HANDLE_VALUE)
	{
		//If not success full display an Error
		if (GetLastError() == ERROR_FILE_NOT_FOUND){

			//Print Error if neccessary
			printf("ERROR: Handle was not attached. Reason: %s not available.\n", portname);

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
		if (!GetCommState(modem->hSerial, &dcbSerialParams))
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
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//Set the parameters and check for their proper application
			if (!SetCommState(modem->hSerial, &dcbSerialParams))
			{
				printf("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				//If everything went fine we're connected
				connected = 1;
				//Flush any remaining characters in the buffers 
				PurgeComm(modem->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				//We wait 2s as the arduino board will be reseting
				//Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}
	
	return connected;
}

void usleep(__int64 usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}
void sleep_seconds(int sec){
	Sleep(1000*sec);
}
#endif



int mmAvailable(struct ModemStream *modem) {
	#ifdef ARDUINO
		return modem->stream->available();
	#endif
	#ifdef LINUX
	
	/*int curr = ftell(modem->in);
	fseek(modem->in, 0, SEEK_END);
	int avail = ftell(modem->in) - curr;
	fseek(modem->in, curr, SEEK_SET);
	return avail;*/
	int bytes;
	ioctl(modem->fd, FIONREAD, &bytes);
	return bytes + modem->peeked;
	#endif
	#ifdef _WIN32
	return modem->status.cbInQue;
	#endif
}

int mmRead(struct ModemStream *modem) {
	#ifdef ARDUINO
		return modem->stream->read();
	#endif
	#ifdef LINUX
		if(modem->peeked){
			modem->peeked = false;
			return modem->peekChar;
		}
		char buf;
		int ret = read(modem->fd, &buf, 1);
		if(ret != 1) return -1;
		return buf;
	#endif
	#ifdef _WIN32
		DWORD bytesRead;
		unsigned int toRead = 1;
		char buffer;

		ClearCommError(modem->hSerial, modem->errors, &modem->status);

		if (modem->status.cbInQue > 0)
			if (ReadFile(modem->hSerial, &buffer, toRead, &bytesRead, NULL) && bytesRead != 0)
				return bytesRead;

		return -1;
	#endif
}

int mmWrite(struct ModemStream* modem, char *buffer, int bytes){
	#ifdef ARDUINO
		return modem->stream->write(buffer, bytes);
	#endif
	#ifdef LINUX
		int written = write(modem->fd, buffer, bytes);
		tcdrain(modem->fd);
		return written;
	#endif
	#ifdef _WIN32
		DWORD bytesSend;
		if (!WriteFile(modem->hSerial, buffer, bytes, &bytesSend, 0))
		{
			ClearCommError(modem->hSerial, modem->errors, &modem->status);
			return -1;
		}
		else
			return (int)bytesSend;
	#endif
}

int mmWriteByte(struct ModemStream* modem, char buffer){
	mmWrite(modem, &buffer, 1);
}

int mmPrint(struct ModemStream *modem, char* buffer) {
	#ifdef ARDUINO
		return modem->stream->print(buffer);
	#endif
	#ifdef LINUX
		int len = strlen(buffer);
		int tmp;
		int origlen = len;
		while(len > 0){
			tmp = write(modem->fd, buffer, len);
			buffer+=tmp;
			len -= tmp;
		}
		tcdrain(modem->fd);
		return origlen;
		//return fputs(buffer, modem->out);
	#endif
	#ifdef _WIN32
		int len = strlen(buffer);
		int origlen = len;
		DWORD bytesSend;
		
		while(len > 0)
			if (WriteFile(modem->hSerial, buffer, len, &bytesSend, 0))
			{
				buffer += (int)bytesSend;
				len -= (int)bytesSend;
			}
			else
				ClearCommError(modem->hSerial, modem->errors, &modem->status);
			
		return origlen;
	#endif
}

int mmPrinti(struct ModemStream *modem, int num) {
	#ifdef ARDUINO
		return modem->stream->print(num);
	#endif
	#ifdef LINUX
		char buff[32]; //potentially dangerous if integers can be represented with more than 32 chars...
		sprintf(buff, "%i", num);
		mmPrint(modem, buff);
		//return fprintf(modem->out, "%d", num);
	#endif
	#ifdef _WIN32
		char buff[32]; //potentially dangerous if integers can be represented with more than 32 chars...
		sprintf_s(buff, "%i", num);
		mmPrint(modem, buff);
	#endif
}

void mmFlush(struct ModemStream *modem) {
	#ifdef ARDUINO
		modem->stream->flush();
	#endif
	#ifdef LINUX
		tcdrain(modem->fd);//ensure data is written.
		modem->peeked = false; //bye bye peek char
		//return true; //not sure why this function is a boolean. /*Arduino returns a boolen from it...I'm not sure either. */
	#endif
	#ifdef _WIN32
		PurgeComm(modem->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
		modem->peeked = false;
		//return true;
	#endif
}

void mmWaitForAvailableData(struct ModemStream *modem){
	while(mmAvailable(modem) <= 0){
		usleep(10);
	}
}

bool mmFindChar(struct ModemStream *modem, char c) {
	while(mmRead(modem) != c) ;
	return true;
	//sleep_millis(streamTimeout);
	while (mmAvailable(modem) > 0) {
		if (mmRead(modem) == c) {
			return true;
		}
	}
	return false;
}

//linux version only works on strings less than 255 characters!!!
bool mmFind(struct ModemStream *modem, char* target) {
	#ifdef ARDUINO
		return modem->stream->find(target);
	#endif
	#ifdef LINUX
		//printf("trying to find %s: ", target);
		//implementation of Knuth-Morris-Pratt
		int j = 0;
		int len = strlen(target);
		char *overlap = alloca(len);
		overlap[0] = 0;
		for(int i = 0; i < len; i++){
			overlap[i+1] = overlap[i] + 1;
			while(overlap[i+1] > 0 && target[i] != target[overlap[i+1]-1])
				overlap[i+1] = overlap[overlap[i+1]-1]+1;
		}
		overlap[0] = 0;
		while(1){
			int c = mmRead(modem);
			if(c < 0) {
				//printf("waiting...\n");
				mmWaitForAvailableData(modem);
				continue;
			}
			//putchar(c);
			if(c == target[j]){//mathes next character
				j++;
				if(j == len){
					//printf("Found %s\n", target);
					return 1;
				} 
					
			}else{//mismatch
				j = overlap[j];
				if(j <0) j = 1;
			}
		}
		//fscanf(modem->in, target);
	#endif
	#ifdef _WIN32
		//printf("trying to find %s: ", target);
		//implementation of Knuth-Morris-Pratt
		int j = 0;
		int len = strlen(target);
		char * overlap = malloc(sizeof(char) * len);
		overlap[0] = 0;
		for(int i = 0; i < len; i++){
			overlap[i+1] = overlap[i] + 1;
			while(overlap[i+1] > 0 && target[i] != target[overlap[i+1]-1])
				overlap[i+1] = overlap[overlap[i+1]-1]+1;
		}
		overlap[0] = 0;
		while(1){
			int c = mmRead(modem);
			if(c < 0) {
				//printf("waiting...\n");
				mmWaitForAvailableData(modem);
				continue;
			}
			//putchar(c);
			if(c == target[j]){//mathes next character
				++j;
				if(j == len){
					//printf("Found %s\n", target);
					return 1;
				} 
					
			}else{//mismatch
				j = overlap[j];
				if(j <0) j = 1;
			}
		}
	#endif
}

char mmPeek(struct ModemStream *modem) {
	#ifdef ARDUINO
		return modem->stream->peek();
	#endif
	#ifdef LINUX
		if(modem->peeked) return modem->peekChar;
		int c = mmRead(modem);
		if(c < 0) return -1;
		modem->peekChar = c;
		modem->peeked = true;
		return (char)c;
	#endif
	#ifdef _WIN32
		if(modem->peeked) return modem->peekChar;
		int c = mmRead(modem);
		if(c < 0) return -1;
		modem->peekChar = c;
		modem->peeked = true;
		return (char)c;
	#endif
}

int mmReadBytes(struct ModemStream *modem, char* buffer, int length) {
	#ifdef ARDUINO
		return modem->stream->readBytes(buffer, length);
	#endif
	#ifdef LINUX
		if(modem->peeked){
			buffer[0] = mmRead(modem);
			length--;
			buffer++;
		}
		return read(modem->fd, buffer, length);
	#endif
	#ifdef _WIN32
		int c;
		int temp = 0;
		if(modem->peeked){
			buffer[0] = mmRead(modem);
			--temp;
			++buffer;
			temp++;
		}
		for(int i = 0; i < length; i++){
			c = mmRead(modem);
			if(c < 0) return i;
			buffer[i] = (char)c;
			temp++;
		}
		return temp;
	#endif
}

int mmReadBytesUntil(struct ModemStream *modem, char target, char* buffer, int length) {
	#ifdef ARDUINO
		return modem->stream->readBytesUntil(target, buffer, length);
	#endif
	#ifdef LINUX
		int c;
		for(int i = 0; i < length; i++){
			c = mmRead(modem);
			if(c < 0) return i;
			if(c == target) return i;
			buffer[i] = (char)c;
		}
		return length;
	#endif
	#ifdef _WIN32
		int c;
		for(int i = 0; i < length; i++){
			c = mmRead(modem);
			if(c < 0) return i;
			if(c == target) return i;
			buffer[i] = (char)c;
		}
		return length;
	#endif
}

char mmFindChars(struct ModemStream *modem, char a, char b){
	int c;
	do{
		c = mmRead(modem);
	} while( c != a && c != b);
	return c;
}

long mmParseInt(struct ModemStream *modem) {
	#ifdef ARDUINO
		long ret = modem->stream->parseInt();
		modem->stream->read();
		return ret;
	#endif
	#ifdef LINUX
	long num = 0;
	int c = mmPeek(modem);
	char neg = 0;
	if(c ==  '-') {
		neg = 1;
		mmRead(modem); //
		c = mmPeek(modem); //get next character.
	}
	if(c < '0' || c > '9') return -1; //not sure what the default value should be...
	else{
		do{
			//mmRead(modem); //read last char, since it was useful
			num *= 10;
			num += c-'0';
			do{
				mmRead(modem);
				c = mmPeek(modem);
			}while(c < 0);
			
		} while (c >= '0' && c <= '9');
	}
	if(neg) return -num;
	return num;
	#endif
	#ifdef _WIN32
	long num = 0;
	int c = mmPeek(modem);
	char neg = 0;
	if(c ==  '-') {
		neg = 1;
		mmRead(modem); //
		c = mmPeek(modem); //get next character.
	}
	if(c < '0' || c > '9') return -1; //not sure what the default value should be...
	else{
		do{
			//mmRead(modem); //read last char, since it was useful
			num *= 10;
			num += c-'0';
			do{
				mmRead(modem);
				c = mmPeek(modem);
			}while(c < 0);
			
		} while (c >= '0' && c <= '9');
	}
	if(neg) return -num;
	return num;
	#endif
}
