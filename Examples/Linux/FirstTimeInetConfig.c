#include "cInet.c"

int main(){
	struct ModemInterface *mm = mmCreate(mm);
	if(mmInit(mm)){
		if(mmInitGPRS(mm)){
			printf("Successfully initialized.\n");
			delete mm;
			return 0;
		}else{
			printf("Initialization failed.\n");
			delete mm;
			return -1;
		}
	}else{
		printf("Unable to initialize connection to modem.\n");
		delete mm;
		return -1;
	}
}