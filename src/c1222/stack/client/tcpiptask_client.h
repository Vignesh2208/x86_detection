#include <winsock2.h>
#include <stdio.h>




extern void DoSleep(unsigned long ms);

extern unsigned char doCMProcessReceivedStreamBytes(char*, int);

extern unsigned long C1222Misc_GetFreeRunningTimeInMS(void);
extern void SetServerPort(unsigned long server_port);
extern void NoteSeparateReply(void);
extern Boolean TcpIpSend_to_Server(void);