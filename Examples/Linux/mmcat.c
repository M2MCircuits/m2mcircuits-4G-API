#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "cInet.h"

#include <stdio.h>

#include "cSMS.h"
int main(int argc, char **argv){
	if(argc < 2) {
		printf("Usage: mmcat [-u] address port\n");
		return 0;
	}
	
	if(argc == 3){
		
	}
	else if(argc == 4){
		if(argv[1][1] != 'u'){
			printf("Unrecognized option %s", argv[1]);
			printf("Usage: mmcat [-u] address port\n");
		return 0;
		}
	}else{
		printf("bad input\n");
		printf("Usage: mmcat [-u] address port\n");
		return 0;
	}
	struct ModemInterface *mm = mmCreate("/dev/ttyACM0");
	if(!mmInit(mm)){
		printf("Unable to initialize modem.");
		return 0;
	}
	if(!mmAttachGPRS(mm)){
		printf("Unable to attach to internet, have you run the first time configuration?\n");
		return 0;
	}
	mmSocketClose(mm, 0); //make sure at least one socket is closed.
	int sid = mmSocketOpen(mm, argc==3);
	if(sid < 0){
		printf("Failed to create socket.\n");
		return 0;
	}
	if(!mmSocketConnect(mm, sid, argv[argc-2], atoi(argv[argc-1]))){
		printf("Could not connect to remote host.\n");
		return 0;
	}
	if(!mmEnterDirectLink(mm, sid)){
		printf("failed to enter direct link mode.");
	}
    fd_set rfds;
    struct timeval tv;
    int retval;
	char buff;
	
	//not the best implementation, ideally you would want to read data in larger chunks, but without threading, it's difficult to provide that functionality.  especially since 
	//cstdin buffers lines...
	while(1){
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 10;
		retval = select(1, &rfds, NULL, NULL, &tv);
		if(retval){
			read(0, buff, 1);
			mmDirectLinkWrite(mm, &buff, 1);
		}
		if(mmDirectLinkAvailable(mm) > 0){
			if(mmDirectLinkRead(mm, &buff, 1) < 0){
				printf("Connection closed.\n");
				free(mm);
				return 0;
			}
			putchar(buff);
		}
	}
}
