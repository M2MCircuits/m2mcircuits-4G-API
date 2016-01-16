#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cModemInterface.h"
#include "cSMS.h"

#define MAX 1024

struct ModemInterface * mm;
bool done = false;
char buffer[MAX];
char message[MAX];
int list[MAX];
int text_count = -1;
int i, count;
char * ptr;
int ch;
bool complete = false;

int main(int argc, char * argv[])
{
	//make modem interface from com port
	mm = mmCreate("COM3");

	printf("Modem created\n");

	//initialize modem interface
	if (mmInit(mm))
	{
		//initialized
		printf("Modem initialized\n");

		//check if modem available
		if (mmAvailable(mm->modem))
		{

			//check if connected to network
			//if (mmCheckForConnection(mm))
			//	{

			while (!done)
			{
				PurgeComm(mm->modem->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

				printf("\n 1 - View SMS list\n");
				printf(" 2 - View SMS by ID\n");
				printf(" 3 - Send SMS\n");
				printf(" 4 - Delete SMS by ID\n");
				printf(" 5 - End program\n");
				printf(" What do you want to do? ");

				scanf("%s", buffer);

				switch (buffer[0])
				{
				case '1':
					text_count = mmGetTextLists(mm, list, MAX);
					if (text_count > 0)
					{
						printf("Text list:\n");
						for (i = 0; i < text_count; i++)
							printf("%i ", list[i]);
						printf("\n");
					}
					else
						if (text_count == 0)
							printf(" No SMS currently\n");
						else
							printf(" Unable to get SMS list\n");
					break;
				case '2':
					printf(" Please enter ID of SMS: ");
					scanf("%s", buffer);

					//convert string to int
					i = (int)strtol((const char *)buffer, &ptr, 10);

					//if valid integer
					if (i > 0)
						if (mmRetrieveTextMessage(mm, buffer, MAX, i))
							printf("Message: %s\n", buffer);
						else
							printf("Unable to retrieve message %i\n", i);
					else
						printf("Invalid id\n");

					break;

				case '3':
					printf(" Please enter Recipient (Max 50): ");
					scanf("%s", buffer);

					while ((ch = getchar()) != EOF && ch != '\n');

					printf(" Please enter message (Max 160): ");
					fgets(message, 1024, stdin);

					//while ((ch = getchar()) != EOF && ch != '\n');

					//if (buffer.size() > RECIPIENT_LIMIT)
					//buffer.resize(RECIPIENT_LIMIT);

					if (strlen(message) > 160)
						message[160] = 0;

					if (mmSendTextMessage(mm, buffer, message))
						printf(" Message sent\n");
					else
						printf(" Unable to send message\n");
					break;

				case '4':
					printf(" Please enter ID of SMS: ");
					scanf("%s", buffer);

					while ((ch = getchar()) != EOF && ch != '\n');

					//convert string to int
					i = (int)strtol((const char *)buffer, &ptr, 10);

					//if valid integer
					if (i > 0)
					{
						printf(" Are you sure you want to delete? (Y/N) ");
						scanf("%s", buffer);
						while ((ch = getchar()) != EOF && ch != '\n');

						switch (buffer[0])
						{
						case 'n':
						case 'N':
							break;
						case 'y':
						case 'Y':
							if (mmDeleteTextMessage(mm, i))
								printf(" Message was deleted\n");
							else
								printf(" Unable to delete message\n");
							break;
						default:
							break;
						}
					}
					else
						printf("Invalid id\n");

					break;
				case '5':
					done = true;
					break;
				default:
					printf(" Invalid request, please try again\n");
				}
			}


		}

	}
	else
	{
		printf("Unable to initialize modem\n");
		return -1;
	}



	return 0;
}