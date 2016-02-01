%include "cstring.i"
%include "carrays.i";
%newobject mmCreate;
%module MakerModem
%cstring_output_maxsize(buffer, length);
%array_functions(int, intArray);
%{
#include <stdbool.h>
//ModemInterface
extern struct ModemInterface;
extern struct ModemInterface * mmCreate(char* blkdvc);
extern bool mmInit(struct ModemInterface* interf);
extern bool mmCheckForConnection(struct ModemInterface* interf);
extern bool mmCheckForOkay(struct ModemInterface* interface);
extern bool mmSendCommandCheckForOkay(struct ModemInterface* interf, char* cmd);
extern void mmSendCommand(struct ModemInterface* interf, char* cmd);
extern int mmGetResponse(struct ModemInterface* interf, char* buffer, int length);
extern void mmFlushBuffer(struct ModemInterface* interf);
extern void mmLogDebug(char* debug, int priority);

//SMS

extern bool mmSendTextMessage(struct ModemInterface* interface, char *num, char* buffer);
extern int mmGetTextLists(struct ModemInterface* interface, int* ids, int length);
extern int mmRetrieveTextMessage(struct ModemInterface* interface, char* buffer, int length, int id);
extern bool mmRetrievePhoneNumber(struct ModemInterface* interface, char* buffer, int length, int id);
extern bool mmRetrieveTimestamp(struct ModemInterface* interface, char* buffer, int length, int id);
extern bool mmDeleteTextMessage(struct ModemInterface* interface, int id);

//LTE

extern int mmGetManufacturer(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetModel(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetFirmware(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetIMEI(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetICCID(struct ModemInterface* interface, char* buffer, int length);
extern bool mmGetCellData(struct ModemInterface* interface, char* buffer, int length);

//inet

extern bool mmIsGPRSAttached(struct ModemInterface *iface);
extern bool mmAttachGPRS(struct ModemInterface *iface);
extern bool mmInitGPRS(struct ModemInterface *iface);
extern int mmSocketOpen(struct ModemInterface *iface, bool tcp);
extern bool mmSocketConnect(struct ModemInterface *iface, int sid, char *dest, int port);
extern int mmSocketAvail(struct ModemInterface *iface, int sid);
extern void mmSocketClose(struct ModemInterface *iface, int sid);
extern int mmSocketWriteBytes(struct ModemInterface *iface, int sid, char *buff, int len);
extern int mmSocketWrite(struct ModemInterface *iface, int sid, char *buffer);
extern int mmSocketRead(struct ModemInterface *iface, int sid, char *buffer, int length);
extern bool mmEnterDirectLink(struct ModemInterface *iface, int sid);
extern bool mmExitDirectLink(struct ModemInterface *iface);
extern int mmDirectLinkRead(struct ModemInterface *iface, char *buffer, int length);
extern int mmDirectLinkWrite(struct ModemInterface *iface, char *buff, int count);
extern int mmDirectLinkAvailable(struct ModemInterface *iface);

extern int testSwig(char *buffer, int length);
%}

extern struct ModemInterface;
extern struct ModemInterface * mmCreate(char* blkdvc);
extern bool mmInit(struct ModemInterface* interf);
extern bool mmCheckForConnection(struct ModemInterface* interf);
extern bool mmCheckForOkay(struct ModemInterface* interface);
extern bool mmSendCommandCheckForOkay(struct ModemInterface* interf, char* cmd);
extern void mmSendCommand(struct ModemInterface* interf, char* cmd);
extern int mmGetResponse(struct ModemInterface* interf, char* buffer, int length);
extern void mmFlushBuffer(struct ModemInterface* interf);
extern void mmLogDebug(char* debug, int priority);

//SMS

extern bool mmSendTextMessage(struct ModemInterface* interface, char *num, char* buffer);
extern int mmGetTextLists(struct ModemInterface* interface, int* ids, int length);
extern int mmRetrieveTextMessage(struct ModemInterface* interface, char* buffer, int length, int id);
extern bool mmRetrievePhoneNumber(struct ModemInterface* interface, char* buffer, int length, int id);
extern bool mmRetrieveTimestamp(struct ModemInterface* interface, char* buffer, int length, int id);
extern bool mmDeleteTextMessage(struct ModemInterface* interface, int id);

//LTE

extern int mmGetManufacturer(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetModel(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetFirmware(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetIMEI(struct ModemInterface* interface, char* buffer, int length);
extern int mmGetICCID(struct ModemInterface* interface, char* buffer, int length);
extern bool mmGetCellData(struct ModemInterface* interface, char* buffer, int length);

//inet

extern bool mmIsGPRSAttached(struct ModemInterface *iface);
extern bool mmAttachGPRS(struct ModemInterface *iface);
extern bool mmInitGPRS(struct ModemInterface *iface);
extern int mmSocketOpen(struct ModemInterface *iface, bool tcp);
extern bool mmSocketConnect(struct ModemInterface *iface, int sid, char *dest, int port);
extern int mmSocketAvail(struct ModemInterface *iface, int sid);
extern void mmSocketClose(struct ModemInterface *iface, int sid);
extern int mmSocketWriteBytes(struct ModemInterface *iface, int sid, char *buff, int len);
extern int mmSocketWrite(struct ModemInterface *iface, int sid, char *buffer);
extern int mmSocketRead(struct ModemInterface *iface, int sid, char *buffer, int length);
extern bool mmEnterDirectLink(struct ModemInterface *iface, int sid);
extern bool mmExitDirectLink(struct ModemInterface *iface);
extern int mmDirectLinkRead(struct ModemInterface *iface, char *buffer, int length);
extern int mmDirectLinkWrite(struct ModemInterface *iface, char *buff, int count);
extern int mmDirectLinkAvailable(struct ModemInterface *iface);


extern int testSwig(char *buffer, int length);