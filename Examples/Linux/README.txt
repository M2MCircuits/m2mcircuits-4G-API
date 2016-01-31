After the beagleboard and the modem are connected,
follow the compilation instructions below to run the examples 

in this directory.

EXAMPLE 1

"Configures modem to use wifi capabilities"

gcc -DLINUX -std=gnu99 FirstTimeInetConfig.c cInet.c 

cModemInterface.c cModemStream.c -o FirstTimeInetConfig


EXAMPLE 2

"Sends an SMS message to a specified phone number"

gcc -DLINUX -std=gnu99 mmsmssend.c cModemInterface.c 

cModemStream.c cSMS.c -o mmsmssend


EXAMPLE 3

"Sends data to a specified address using sockets"

gcc -DLINUX -std=gnu99 mmcat.c cInet.c cModemInterface.c 

cModemStream.c -o mmcat

EXAMPLE 4
"Lists all sent/unsent messages recorded by the modem"

gcc -DLINUX -std=gnu99 mmsmsreadall.c cModemInterface.c 

cModemStream.c cSMS.c -o mmsmsreadall

EXAMPLE 5
"Deletes all sent/unsent messages recorded by the modem"

gcc -DLINUX -std=gnu99 mmsmsdeleteall.c cModemInterface.c 

cModemStream.c cSMS.c -o mmsmsdeleteall
