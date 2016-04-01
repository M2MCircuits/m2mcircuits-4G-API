//Listens for incoming text messages and executes commands depending on their content.
//Compile with gcc -DLINUX -std=gnu99 smsListener.c cModemInterface.c cModemStream.c cSMS.c -o smsListener

//Usage: the program should begin listening on boot. To accomplish this, add the following lines to etc/rc.local:
//	sudo su
//	cd/home/debian/makermodem-master/Examples/Linux (or cd/<your_folder containing smsListener>)
//	./smsListener
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "cSMS.h"

int main(int argc, char **argv){
	//Sets up connection to MakerModem and exits if initialization fails
	struct ModemInterface *mm = mmCreate("/dev/ttyACM0");
	if(!mmInit(mm)){
		printf("Unable to initialize modem.");
		return 0;
	}
	
	int messageids[256];
	int count = mmGetTextLists(mm, messageids, 256);
	char number[11];
	char message[180];
	char coordMessage[180];
	double lat = 32.9851001;
	double longit = -96.74942709999999;
	
	while (true) {
		sleep(1);
		// printf("Time Up!\n");
		
		int newCount = mmGetTextLists(mm, messageids, 256);
		
		//Checks if new messages have come in
		if (newCount > count){
			int diff = newCount - count;
			printf("%i new messages have been received.\n", diff);//Prints to console for testing purposes

			//For every new message received in this loop cycle
			for(int i = count; i < newCount; i++){
				mmRetrievePhoneNumber(mm, number, 11, messageids[i]);
				mmRetrieveTextMessage(mm, message, 180, messageids[i]);
				printf("Message from %s:\n%s\n", number, message); //Prints to console for testing purposes
				
				//Converts message to lowercase to reduce input error
				for(int i = 0; message[i]; i++){
				  message[i] = tolower(message[i]);
				}
	
				//Essentially a switch statement for strings. Responds to the sending number with a text based on the command received.
				if (strcmp(message, "gps") == 0)  //Gives coordinates of GPS location and a Google Maps link
				{
					snprintf(coordMessage, 180, "Lat: %f\tLong: %f\nGoogle Maps Link: http://maps.google.com/?q=%f,%f", lat, longit, lat, longit);
					mmSendTextMessage(mm, number, coordMessage);
				} 
				else if (strcmp(message, "route1") == 0)  //Tells drone to go on route 1
				{
					system(".//route1");
					mmSendTextMessage(mm, number, "Route 1 activated.");
				}
				else {
					mmSendTextMessage(mm, number, "Unrecognized command.");
				}
				
			}
			// mmDeleteTextMessage(mm, messageids[5]);
			count = newCount;
			
		} //end if 
		
	} //end while
	
} 
