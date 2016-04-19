/* Listens for incoming text messages and executes commands depending on their content.
   Accepted messages:
        "route1": follow the pre-set route1 mission on the pixhawk.
	"gps": send back a message containing current GPS.
	"go to <lat> <lon>": go to the specified GPS coordinate.

   Compile: gcc -DLINUX -std=gnu99 smsListener2.c cModemInterface.c cModemStream.c cSMS.c -o smsListener2
   Usage: the program is run on the drone's BB. It should begin listening on boot. To accomplish this, add the following lines to etc/rc.local:
	sudo su
	cd/home/debian/makermodem-master/Examples/Linux (or cd/<your_folder containing smsListener>)
	./smsListener2

*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "cSMS.h"

/* Print the last 5 received messages for testing */
void messagesHead(int count, int* messageIds, struct ModemInterface *mm) {
  char phoneNumber[11];
  char message[180];

  for (int i = count-5; i < count; i++) {
    if (i >= 0) {
      mmRetrievePhoneNumber(mm, phoneNumber, 11, messageIds[i]);
      mmRetrieveTextMessage(mm, message, 180, messageIds[i]);
      printf("Message from %s:\n%s\n", phoneNumber, message);
    }
  }
}

/* Check if a phone number is accepted to send commands.
   
   Always return true for testing
 */
int isNumberValid(char *phoneNumber) {
  return 1;
}

/* Start route1 in pixhawk using mavproxy.py
   TODO: 
          Retrive and follow different missions in the pixhawk
	  Return something to notify end of mission
*/
void followRoute1() {
	printf("\nStart Route 1 Logic\n");
	
	//UART communication
	system("echo BB-UART1 > /sys/devices/bone_capemgr.9/slots");
	
	//start mavproxy
	system("sudo mavproxy.py --master=/dev/ttyO1"); 
	
	// At this point you should have a MAV> prompt
	// Go to AUTO mode, the drone will follow the mission in the pixhawk
	system("mode auto"); 
	
	// Notify end of mission
	printf("\nEnd Route 1 Logic\n");
}

/* Message format: "goto <LAT> <LON> */
void goToGPS(char *message) {
        /* Parse the lattitude and longitude from message*/
  
        // Find 2 spaces and end of string
        int x1;
	x1 = 0;
	while (message[x1] != ' ') {
	  x1 += 1;
	}
	int x2 = x1 + 1;
	while (message[x2] != ' ') {
	  x2 += 1;
	}
	int size = x2 + 1;
	while (message[size] != '\0') {
	  size += 1;
	}

	// Parse lattitude
	char* lat = 0;

	int begin_index = x1+1;
	int end_index = x2;

	int l = end_index - begin_index;
	lat = (char*)malloc(l+1);
	memcpy(lat, message + begin_index, l);
	lat[l] = '\0';
	
	// Parse longitude
	char* lon = 0;

	begin_index = x2+1;
	end_index = size;

	l = end_index - begin_index;
	lon = (char*)malloc(l+1);
	memcpy(lon, message + begin_index, l);
	lon[l] = '\0';

	printf("Lat: %s, Lon: %s\n", lat, lon);	
}

int main(int argc, char **argv){
	//Sets up connection to MakerModem and exits if initialization fails
	struct ModemInterface *mm = mmCreate("/dev/ttyACM0");
	if(!mmInit(mm)){
		printf("Unable to initialize modem.\n");
		return 0;
	} else {
		printf("Successfully initialized.\n");
	}
	
	int messageIds[256];
	int count = mmGetTextLists(mm, messageIds, 256);

	// Print the original text list
	printf("Last messages: \n");
	messagesHead(count, messageIds, mm);
	
	char phoneNumber[11];
	char message[180];
	char coordMessage[180];

	// fake GPS for testing
	double lat = 32.9851001;
	double lon = -96.74942709999999;

	/* Regular Expressions */
	regex_t goToGPSRegext;

	// Go to GPS
	char *goToGPSRegex = "goto*";
	int retiGoTo = regcomp(&goToGPSRegext, goToGPSRegex, 0);
	if (retiGoTo) {
	        fprintf(stderr, "Could not compile regex\n");
	}

	// Listen to SMSs
	printf("\nWaiting for SMS...\n");
	while (true) {
		sleep(1);
		// printf("Time Up!\n");
		
		int newCount = mmGetTextLists(mm, messageIds, 256);
		
		//Checks if new messages have come in
		if (newCount > count){
			int diff = newCount - count;
			printf("%i new messages have been received.\n", diff);//Prints to console for testing purposes

			//For every new message received in this loop cycle
			for(int i = count; i < newCount; i++){
			        mmRetrievePhoneNumber(mm, phoneNumber, 11, messageIds[i]);
				mmRetrieveTextMessage(mm, message, 180, messageIds[i]);
				printf("Message from %s:\n%s\n", phoneNumber, message); //Prints to console for testing purposes

				if (isNumberValid(phoneNumber)) {
				      //Converts message to lowercase to reduce input error
				      for(int i = 0; message[i]; i++){
					    message[i] = tolower(message[i]);
				      }
	
				      //Essentially a switch statement for strings. Responds to the sending number with a text based on the command received.
				      if (strcmp(message, "gps") == 0) {
					    //Gives coordinates of GPS location and a Google Maps link
					    snprintf(coordMessage, 180, "Lat: %f\tLong: %f\nGoogle Maps Link: http://maps.google.com/?q=%f,%f", lat, lon, lat, lon);
					    mmSendTextMessage(mm, phoneNumber, coordMessage);
				      } 
				      else if (strcmp(message, "route1") == 0) {
					    //Tells drone to go on route 1
					    //system(".//route1");
					    mmSendTextMessage(mm, phoneNumber, "Route 1 activated.");
					    followRoute1();
					    mmSendTextMessage(mm, phoneNumber, "Route 1 accomplished.");
				      }
				      else if (regexec(&goToGPSRegext, message, 0, NULL, 0) != REG_NOMATCH) {
					    mmSendTextMessage(mm, phoneNumber, "Going to GPS location (., .).");
					    goToGPS(message);
					    mmSendTextMessage(mm, phoneNumber, "Mission accomplished.");
				      }
				      else {
					    // Unsupported/unrecognized commands
					    mmSendTextMessage(mm, phoneNumber, "Unrecognized command.");
				      }
				}			      
			}
			// mmDeleteTextMessage(mm, messageIds[5]);
			count = newCount;
			
		} //end if 
		
	} //end while
	
} 
