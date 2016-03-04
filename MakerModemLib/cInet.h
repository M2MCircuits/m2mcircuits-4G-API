#ifndef C_MMINET_H
#define C_MMINET_H
#include "cModemInterface.h"

//Set the APN
bool mmSetAPN(struct ModemInterface *iface, char apnname[]);
//Check if device is connected to the internet.
bool mmIsGPRSAttached(struct ModemInterface *iface);
//Connect device to the internet
bool mmAttachGPRS(struct ModemInterface *iface);
//First time internet configuration (run at least once, but multiple times does not hurt).
bool mmInitGPRS(struct ModemInterface *iface);

//open socket, tcp or udp
int mmSocketOpen(struct ModemInterface *iface, bool tcp);
//connect to a server
bool mmSocketConnect(struct ModemInterface *iface, int sid, char *dest, int port);
//check if data is available
int mmSocketAvail(struct ModemInterface *iface, int sid);
//close socket
void mmSocketClose(struct ModemInterface *iface, int sid);

//write bytes, returns number of bytes read
int mmSocketWriteBytes(struct ModemInterface *iface, int sid, char *buff, int len);
//write string
int mmSocketWrite(struct ModemInterface *iface, int sid, char *buff);
//read bytes, returns number of bytes read
int mmSocketRead(struct ModemInterface *iface, int sid, char *buff, int maxlen);

//enter direct link mode.  allows higher throughput to the socket endpoint, but can't run any other commands
bool mmEnterDirectLink(struct ModemInterface *iface, int sid);
//exit direct link mode, other commands can now be run
bool mmExitDirectLink(struct ModemInterface *iface);
//read bytes from direct link mode, use this, not the commands above while in direct link mode
int mmDirectLinkRead(struct ModemInterface *iface, char *buff, int count);
//same as above, but writing
int mmDirectLinkWrite(struct ModemInterface *iface, char *buff, int count);
//check how much data available
int mmDirectLinkAvailable(struct ModemInterface *iface);

#endif