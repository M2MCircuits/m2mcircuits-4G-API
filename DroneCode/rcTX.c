// rcTX.c          RUNS ON BASE STATION/GROUND STATION
// Functionality:
// runs as a transmitter sending rc control data over UDP to drone side
// gets data from Launchpad over UART2

// compile line
// gcc -DLINUX -std=gnu99 rcTX.c -o rcTX

// Note Baud Rate = 115200
// Beaglebone Black UART2       RX      TX      CTS     RTS     Device
//                              P9_22   P9_21   P8_37   P8_38   /dev/ttyO2

// TI LanchPad MSP430 UART      RX      TX      CTS     RTS     Device


//INITIAL REQUIRED SETUP FOR BEAGLEBOARD: go to etc/rc.local and add line stty -F /dev/ttyO2 115200 to set baud rate
//*****************************// INCLUDES //*****************************//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>			// used for read()
#include <sys/types.h>      /* system type definitions */
#include <sys/socket.h>     /* network system functions */
#include <netinet/in.h>     /* protocol & struct definitions */

//****************************// # DEFINES //****************************//
#define RC_BYTES 9
#define BUF_SIZE 1024
#define DRONE_IP "130.127.24.92"			// ip address @ Drone end
#define DRONE_PORT 60000					// ip port @ Drone end

//************************// FUNC DECLARATIONS //************************//
int initUART(void);
int UDPsocket(void);
void setupUDP(void);

//*****************************// GLOBALS //*****************************//
char buff[RC_BYTES];						// 9 bytes read from Launchpad
struct sockaddr_in addr_send;
int sock_send;
char text[80],buf[BUF_SIZE];
int send_len,bytes_sent;



int main(int argc, char *argv[])
{
	int rcBytes;
	int uart2 = initUART();		// initialize UART2 for reading from Launchpad
	int check = UDPsocket();	// create a UDP socket

	if (uart2 && check)
	{
		setupUDP();		// setup the UDP socket
		while(1)
		{
			rcBytes = read(uart2, buff, 9);		// check if there is data from Lanchpad
			if (rcBytes)	// if there is UART data, send it out through UDP socket
			{
				bytes_sent=sendto(sock_send, buff, rcBytes, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));
				printf("DEBUGGING: rcBytes=%i",rcBytes);
			}
			
			// test code for sending a string
			// printf("Send? ");
			// scanf("%s",text);
			// if (strcmp(text,"quit") == 0)
			// 	break;
			// strcpy(buf,text);
			// send_len=strlen(text);
			// bytes_sent=sendto(sock_send, buf, send_len, 0,(struct sockaddr *) &addr_send, sizeof(addr_send));
		}
		close(sock_send);
	}
	else
		{ exit(0); }
	return 0; 
}


// Used to initialize UART2 comms for writing to Launchpad
int initUART()
{
	// set the UART baud rates on UART2 and UART 3 to 115200
	// stty -F /dev/ttyO2 115200
	// stty -F /dev/tty02/115200

	//open serial port UART2 @ 115200 baud for reading --- RX on BeagleBoard from LaunchPad
	int fd = open("/dev/ttyO2", O_RDONLY | O_NOCTTY); //
	if ( fd )
	{
		printf("opened /dev/ttyO2 for reading at 115200\n");
	 	return fd;
	}
	else
	{
		printf("Unable to initialize UART2 port for reading\n");
		return fd;
	}
}


// Used to create a UDP socket for sending data
int UDPsocket(void)
{
	sock_send=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_send < 0)
	{
		printf("socket() failed\n");
		return 0;
	}
	else
		return 1;
}


// Used to fill out the address structure for sending data
void setupUDP(void)
{
	memset(&addr_send, 0, sizeof(addr_send));			/* clr structure */
	addr_send.sin_family = AF_INET;						/* address family */
	addr_send.sin_addr.s_addr = inet_addr(DRONE_IP);
	addr_send.sin_port = htons((unsigned short)DRONE_PORT);
}


