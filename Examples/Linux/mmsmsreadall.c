#include <stdio.h>

#include "cSMS.h"
int main(int argc, char **argv){
	struct ModemInterface *mm = mmCreate("/dev/ttyACM0");
	if(!mmInit(mm)){
		printf("Unable to initialize modem.");
		return 0;
	}
	int messageids[256];
	int count = mmGetTextLists(mm, messageids, 256);
	char number[11];
	char message[180];
	printf("%i messages: \n", count);
	for(int i = 0; i < count; i++){
		mmRetrievePhoneNumber(mm, number, 11, messageids[i]);
		mmRetrieveTextMessage(mm, message, 180, messageids[i]);
		printf("Message from %s:\n%s\n", number, message);
	}
	
}