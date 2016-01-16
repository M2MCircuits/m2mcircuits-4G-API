Class Structure:
cModemStream provides low-level functionality in communicating with the modem. It is modeled after the Arduino Stream class and provides most of the methods that Stream does. Almost all platform-specific code has been abstracted into the cModemStream class.
cModemInterface provides higher-level functionality based on cModemStream. Here, methods such as mmSendCommand and mmCheckForOkay allow for interaction with the modem without having to parse the stream directly. When retrieving responses from the modem with mmGetResponse, pass in a char* and a length, and the method will put the response into the buffer.
cLTE provides functionality for retrieving information about the modem, such as firmware version and ccid. It also provides a function to retrieve cell data (mcc, mnc, lac, ci) from the network if the modem is connected.
cSMS provides text messaging functionality. getTextList populates an int array with the ids of all received text messages. Those ids are used in conjunction with the message, timestamp, phone number, and delete methods to specify which text message to act on. mmSendTextMessage will send a message based on a phone number and message passed in by the user. Message length is a maximum of 160 characters.

To use this library, include the appropriate head file with the functionality you need.  IE, if you want to send a text message, use cSMS.h

Arduino Setup:
When initializing the Modem Interface with mmCreate(Stream*), either a Serial or a Software Serial can be used. Make sure to fully initialize the Stream with Stream.begin(baud) before passing it in to mmCreate. The modem should detect baudrate, but 9600 works well.

Using Arduino Libraries:
All libraries in Arduino must be put into a folder with the name of the files as its name (i.e. cLTE.h and cLTE.cpp should be put into a folder called cLTE). In addition, the library endings should be changed from .c to .cpp in order to work with Arduino.

Serial Connection:
The pins in the header are numbered right to left, with the top row containing even numbers and bottom row containing odd numbers:

 9 7 5 3 1
10 8 6 4 2

To set up a serial connection: Rx should be plugged into pin 7, Tx should be plugged into pin 6, V should be plugged into 10, and Ground should be plugged into either 3 or 4. 
NOTE: The Modem has its Rx and Tx conventions backwards. Pin 7 is listed as Rx, but it is actually the Tx port.


Example initialization code:

struct ModemInterface *mm = mmCreate("/dev/ttyACM0"); //linux
struct ModemInterface *mm = mmCreate("COM5"); //windows
struct ModemInterface *mm = mmCreate(mySerial); //arduino

mmInit(mm);

if a function returns -1, that indicates an error occurred.  

See examples for more use cases.  

Notes on direct link mode

Direct link mode offers higher throughput that using the other socket commands, at the price of not being able to send any commands (IE, text messaging) while in direct link mode.  
If the remote endpoint disconnects (applies to TCP only) while in direct link mode, the last read function will not return an error.  However the next command will return -1.  
This behavior matches file IO on linux using FILE*.  If the last read command returned error instead, then the user would not know how much data was read before the connection terminated.  
If the remote endpoint sends DISCONNECT, the library will consider it a disconnection as this is the sentinel the modem uses to indicate a disonnection.

When compiling on linux, please ensure LINUX is defined.  Adding -DLINUX to you gcc command accomplishes this.  
For Arduino, #define ARDUINO
Windows users should not have to worry as _WIN32 is defined by default.
