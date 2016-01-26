/* Abstraction class for the ModemStream
 * General stream functions used in ModemInterface class
 * Uses ifdefs to cut down on program length
 */
 
#include "ModemStream.h"

#ifdef ARDUINO
ModemStream::ModemStream(Stream* stream) {
	this->stream = stream;
}
#endif

#ifdef LINUX
ModemStream::ModemStream() {

}
#endif

int ModemStream::available() {
	#ifdef ARDUINO
		return this->stream->available();
	#endif
	#ifdef LINUX
	
	#endif
}

char ModemStream::read() {
	#ifdef ARDUINO
		return this->stream->read();
	#endif
	#ifdef LINUX
	
	#endif
}

int ModemStream::print(char* buffer) {
	#ifdef ARDUINO
		return this->stream->print(buffer);
	#endif
	#ifdef LINUX
	
	#endif
}

int ModemStream::print(int num) {
	#ifdef ARDUINO
		return this->stream->print(num);
	#endif
	#ifdef LINUX
	
	#endif
}

bool ModemStream::flush() {
	#ifdef ARDUINO
		this->stream->flush();
	#endif
	#ifdef LINUX
	
	#endif
}

bool ModemStream::find(char* target) {
	#ifdef ARDUINO
		return this->stream->find(target);
	#endif
	#ifdef LINUX
	
	#endif
}

bool ModemStream::findUntil(char* target, char* term) {
	#ifdef ARDUINO
		return this->stream->findUntil(target,term);
	#endif
	#ifdef LINUX
	
	#endif
}

char ModemStream::peek() {
	#ifdef ARDUINO
		return this->stream->peek();
	#endif
	#ifdef LINUX
	
	#endif
}

int ModemStream::readBytes(char* buffer, int length) {
	#ifdef ARDUINO
		return this->stream->readBytes(buffer, length);
	#endif
	#ifdef LINUX
	
	#endif
}

int ModemStream::readBytesUntil(char target, char* buffer, int length) {
	#ifdef ARDUINO
		return this->stream->readBytesUntil(target, buffer, length);
	#endif
	#ifdef LINUX
	
	#endif
}
/* Not using strings for now
String ModemStream::readString() {
	#ifdef ARDUINO
		return this->stream.readString();
	#endif
	#ifdef LINUX
	
	#endif
}

String ModemStream::readBytesUntil(char term) {
	#ifdef ARDUINO
		return this->stream.readStringUntil(term);
	#endif
	#ifdef LINUX
	
	#endif
}
*/
long ModemStream::parseInt() {
	#ifdef ARDUINO
		return this->stream->parseInt();
	#endif
	#ifdef LINUX
	
	#endif
}

float ModemStream::parseFloat() {
	#ifdef ARDUINO
		return this->stream->parseFloat();
	#endif
	#ifdef LINUX
	
	#endif
}
