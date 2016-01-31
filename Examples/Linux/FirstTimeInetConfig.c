#include "cInet.h"

int main(){
	struct ModemInterface *mm = mmCreate("/dev/ttyACM0"); //linux
	if(mmInit(mm)){
		if(mmInitGPRS(mm)){
			printf("Successfully initialized.\n");
			free(mm);
			return 0;
		}else{
			printf("Initialization failed.\n");
			free(mm);
			return -1;
		}
	}else{
		printf("Unable to initialize connection to modem.\n");
		free(mm);	
		return -1;
	}
}
