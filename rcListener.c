// rcSignalListener.c
// Functionality: used to capture 9 bytes of data from makermodem over
// usb/UART3 connection and then retransmit them to a MSP430 over UART2 
// Three basic steps
// 1. make interface connection to maker modem
// 2. begin listening for data from maker modem
// 3. if there is any data from the modem, send it out to MSP430 via UART2

// Note Baud Rate = 115200
// Beaglebone Black UART2 		RX 		TX 		CTS 	RTS 	Device
// 								P9_22	P9_21	P8_37	P8_38	/dev/ttyO2

// TI LanchPad MSP430 UART 		RX 		TX 		CTS 	RTS 	Device

// compile line
// gcc -DLINUX -std=gnu99 rcListener.c cModemInterface.c cModemStream.c -o rc


// Maker Modem library header files
// #define LINUX
#include "cModemInterface.h"

// Standard header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


int init(void);
//*****************************// GLOBALS //*****************************//
char buff[5];
// const char usb = "/dev/ttyACM0";
int modem;
int fd;
int bytes;
struct ModemInterface *mm;
// struct ModemStream *mStream;
//************************************************************************


int main() {

	// buff[0] = 0xAA;
	int ok = init();		// initalize all communication parameters

	// if all initializations are ok begin reading the rc data from the maker
	// modem and writing/sending that data to the MSP430 over UARTS continously
	if (ok)
	{
		// mStream = mm.modem;
		while(1)
		{
			bytes = mmAvailable(mm->modem);
			if (bytes > 0)
			{
				// read the bytes from the maker modem
				mmReadBytes(mm->modem, buff, bytes);
				// write the bytes to the MSP430
				write(fd, buff, 1);
				printf("writing 5 to ttyO2\n");
			}
		}
	}
	return 0;
}

// function used to initialize all communicaiton parameters to devices
int init()
{
	int check1, check2, check3;
	// set the UART baud rates on UART2 and UART 3 to 115200
	// stty -F /dev/ttyO2 115200
	// stty -F /dev/tty02/115200

	// sets up connection to MakerModem and exits if initialization fails
	mm = mmCreate("/dev/ttyACM0");
	if( mmInit(mm) ){
		check1 = 1;
		printf("modem interface initialized\n");
	}
	else{
		check1 = 0;
		printf("Unable to initialize modem.");
	}


	// open usb serial port @ 115200 baud for reading --- maker modem to RX on BeagleBoard
	modem = mmCommInit("/dev/ttyACM0", 115200);
	if ( modem ){
		check2 = 1;
		printf("Serial port with maker modem opened at 115200 baud\n");
	}
	else{
		check2 = 0;
		printf("Unable to initialize serial port with maker modem\n");
	}


	// open serial port UART2 @ 115200 baud for writing --- TX on BeagleBoard to MSP430
	int fd = open("/dev/ttyO2", O_WRONLY | O_NOCTTY); // 
	if ( fd ){
		printf("opened /dev/ttyO2 for writing at 115200\n");
		check3 = 1;
	}
	else{
		check3 = 0;
		printf("Unable to initialize UART2 port for writing\n");
	}

	if (check1 && check2 && check3)
	{ return 1; }
	else
		return 0;
}


