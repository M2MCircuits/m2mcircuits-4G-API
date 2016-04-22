// rcTX.c           RUNS ON DRONE SIDE
// Functionality:
// runs as a reciever listening for rc control data over UDP on drone side
// sends data to Launchpad over UART2

// compile line
// gcc -DLINUX -std=gnu99 rcRX.c -o rcRX

// Note Baud Rate = 115200
// Beaglebone Black UART2       RX      TX      CTS     RTS     Device
//                              P9_22   P9_21   P8_37   P8_38   /dev/ttyO2

// TI LanchPad MSP430 UART      RX      TX      CTS     RTS     Device


//*****************************// INCLUDES //*****************************//
#include <arpa/inet.h>
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
#define DRONE_PORT 60000					// ip port @ Drone end

//************************// FUNC DECLARATIONS //************************//
int initUART(void);
int UDPsocket(void);
void setupUDP(void);

//*****************************// GLOBALS //*****************************//
char buff[RC_BYTES];						// 9 bytes read from Launchpad
int sock_recv;
struct sockaddr_in my_addr;
int i;
fd_set readfds;
struct timeval timeout={0,0};
int incoming_len;
struct sockaddr remote_addr;
int recv_msg_size;
char buf[BUF_SIZE];
int select_ret;



int main(int argc, char *argv[])
{

	int rcBytes;
	int uart2 = initUART();		// initialize UART2 for reading from Launchpad
	int check = UDPsocket();	// create a UDP socket

	if (rcBytes && check)
	{
		setupUDP();		// setup the UDP socket
		while(1)
		{
			do
			{
				FD_ZERO(&readfds); /* zero out socket set */
				FD_SET(sock_recv,&readfds); /* add socket to listen to */
				select_ret=select(sock_recv+1,&readfds,NULL,NULL,&timeout);
				if (select_ret > 0) /* anything arrive on any socket? */
				{
					incoming_len=sizeof(remote_addr); /* who sent to us? */
					recv_msg_size=recvfrom(sock_recv,buf,BUF_SIZE,0,(struct sockaddr *)&remote_addr,&incoming_len);
					if (recv_msg_size > 0) /* what was sent? */
					{
						buf[recv_msg_size]='\0';
						// printf("From %s received: %s\n", inet_ntoa(remote_addr.sin_addr) , buf);
						printf("revieved:  %s\n", buf);
						write(uart2, buf, 1);	// Send rc Data bytes to Launch Pad via UART2
					}
				}
			}
			while (select_ret > 0);
				if (strcmp(buf,"shutdown") == 0)
					break;
		}
		close(sock_recv);
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

	//open serial port UART2 @ 115200 baud for writing --- TX on BeagleBoard to LaunchPad
	int fd = open("/dev/ttyO2", O_WRONLY | O_NOCTTY); //
	if ( fd )
	{
		printf("opened /dev/ttyO2 for writing at 115200\n");
	 	return fd;
	}
	else
	{
		printf("Unable to initialize UART2 port for writing\n");
		return fd;
	}
}


// Used to create a UDP socket for listening/recieving data
int UDPsocket(void)
{
	sock_recv=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_recv < 0)
	{
		printf("socket() failed\n");
		return 0;
	}
	else
		return 1;
}


// Used to fill out the address structure for receiving data
void setupUDP(void)
{
	/* make local address structure */
	memset(&my_addr, 0, sizeof (my_addr)); /* clr structure */
	my_addr.sin_family = AF_INET; /* address family */
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* current IP */
	my_addr.sin_port = htons((unsigned short)DRONE_PORT);
	
	/* bind socket to the local address */
	i=bind(sock_recv, (struct sockaddr *) &my_addr, sizeof (my_addr));

	if (i < 0)
	{
		printf("bind() failed\n");
		exit(0);
	}
}


