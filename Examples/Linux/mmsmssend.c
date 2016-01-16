#include <stdio.h>

#include "cSMS.h"
int main(int argc, char **argv){
	if(argc != 3) {
		printf("Usage: mmsmssend \"number\" \"message\"\n");
		return 0;
	}
	struct ModemInterface *mm = mmCreate("/dev/ttyACM0");
	if(!mmInit(mm)){
		printf("Unable to initialize modem.");
		return 0;
	}
	if(mmSendTextMessage(mm, argv[1], argv[2])){
		printf("Message sent.\n");
	}else{
		printf("Unable to send message.\n");
	}
	
}