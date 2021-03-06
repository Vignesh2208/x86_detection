#include "c1222stack.h"
#include "c1219table.h"
#include "epsem.h"
#include "acsemsg.h"

#define REQUEST_TYPE_IDENT 1
#define REQUEST_TYPE_READ 2
#define REQUEST_TYPE_WRITE 3
#define REQUEST_TYPE_LOGON 4
#define REQUEST_TYPE_LOGOFF 5
#define REQUEST_TYPE_TERMINATE 6
#define REQUEST_TYPE_DISCONNECT 7
#define REQUEST_TYPE_WAIT 8
#define REQUEST_TYPE_RESOLVE 9
#define REQUEST_TYPE_TRACE 10
#define MAX_EPSEM_BODY_SIZE 100
#define MAX_ACSE_HDR_MSG_SIZE 100
#define MAX_ACSE_MSG_SIZE 200

#define TRUE 1
#define FALSE 0
typedef struct C1222SClientTag
{
    C1222Stack* pStack;
       
    
    Unsigned8* requestMsgbuffer;
	Unsigned8* responseMsgbuffer;
	Unsigned8* Called_Aptitle;
	Unsigned8 Called_Aptitle_length;
    Unsigned8* myAptitle;
	Unsigned8 myAptitle_length;
	Unsigned8* myMRAptitle;
	Unsigned8 myMRAptitle_length;
    Unsigned16 CallingAp_InvocationId;
	Unsigned8* Destination_Native_Address;
	Unsigned16 Destination_Native_Address_length;
    
} C1222Client;

void setmyApTitle(C1222Client * pClient, Unsigned8* Aptitle, Unsigned8 length);
void setmyMRApTitle(C1222Client * pClient, Unsigned8* Aptitle, Unsigned8 length);
void setCalled_Aptitle(C1222Client * pClient, Unsigned8 * Called_Aptitle, Unsigned8 Called_Aptitle_length);
void setDestination_Native_Address(C1222Client * pClient, Unsigned8 * Destination_Native_Address, Unsigned16 Destination_Native_Address_length);
void setCallingApInvocationId(C1222Client * pClient, Unsigned16 CallingAp_InvocationId);
Unsigned16 ComposeIdentRequest_Epsem(Unsigned8* request);
Unsigned16 ComposeReadRequest_Epsem(Unsigned8* request, Unsigned8* tableId, Boolean isFullRead, Unsigned8* offset, Unsigned8* octet_count);
Unsigned16 ComposeWriteRequest_Epsem(Unsigned8* request, Unsigned8* tableId, Boolean isFullWrite, Unsigned8* offset, Unsigned8* tabledata, Unsigned16 tabledata_length);
Unsigned16 ComposeLogonRequest_Epsem(Unsigned8* request, Unsigned8 * User_Info, Unsigned8 * User_id, Unsigned16 session_idle_timeout);
Unsigned16 ComposeLogoffRequest_Epsem(Unsigned8* request);
Unsigned16 ComposeTerminateRequest_Epsem(Unsigned8* request);
Unsigned16 ComposeDisconnectRequest_Epsem(Unsigned8* request);
Unsigned16 ComposeWaitRequest_Epsem(Unsigned8* request, Unsigned8 duration);
Unsigned16 ComposeResolveRequest_Epsem(Unsigned8* request, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length);
Unsigned16 ComposeTraceRequest_Epsem(Unsigned8* request, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length);
Unsigned16 Create_ACSE_Header(ACSE_Message* pRequestMsg, Unsigned8* Called_Aptitle, Unsigned8 length_of_called_Aptitle, Unsigned8* Calling_Aptitle, Unsigned8 length_of_callng_Aptitle, Unsigned16 CallingAp_InvocationId, C1222StandardVersion standardVersion) ;
Boolean CreateRequestMessage_Ident(ACSE_Message * pRequestMsg);
Boolean CreateRequestMessage_Read(ACSE_Message * pRequestMsg, Unsigned8* tableId, Boolean isFullRead, Unsigned8* offset, Unsigned8* octet_count);
Boolean CreateRequestMessage_Write(ACSE_Message * pRequestMsg, Unsigned8* tableId, Boolean isFullWrite, Unsigned8* offset, Unsigned8* tabledata, Unsigned16 tabledata_length);
Boolean CreateRequestMessage_Logon(ACSE_Message * pRequestMsg, Unsigned8 * User_Info, Unsigned8 * User_id, Unsigned16 session_idle_timeout);
Boolean CreateRequestMessage_Logoff(ACSE_Message * pRequestMsg);
Boolean CreateRequestMessage_Terminate(ACSE_Message * pRequestMsg);
Boolean CreateRequestMessage_Disconnect(ACSE_Message * pRequestMsg);
Boolean CreateRequestMessage_Wait(ACSE_Message * pRequestMsg,Unsigned8 duration);
Boolean CreateRequestMessage_Resolve(ACSE_Message * pRequestMsg, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length);
Boolean CreateRequestMessage_Trace(ACSE_Message * pRequestMsg, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length);
Boolean epsemControl_Checks(Unsigned8 epsemControl, Boolean * isEd_Class_included);
Boolean Validate_Ident_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Read_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Write_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Logon_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Logoff_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Terminate_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Disconnect_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Wait_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Resolve_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Validate_Trace_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included);
Boolean Check_Received_Message_Semantics(ACSE_Message* pReceived_Msg, C1222StandardVersion standardVersion, Unsigned8 Type_of_sent_message);
C1222Client_Init(C1222Client * pClient, C1222Stack * pStack, Unsigned8 * requestMsgbuffer, Unsigned8* responseMsgbuffer);
Boolean C1222Client_Send_Message(C1222Stack * pStack, ACSE_Message* Msg, Unsigned8* Destination_Native_Address, Unsigned16 Destination_Native_Address_length);
Boolean C1222Client_Send_Ident_Request(C1222Client * pClient);
Boolean C1222Client_Send_Read_Request(C1222Client * pClient,Unsigned8* tableId, Boolean isFullRead, Unsigned8* offset, Unsigned8* octet_count);
Boolean C1222Client_Send_Write_Request(C1222Client * pClient,Unsigned8* tableId, Boolean isFullWrite, Unsigned8* offset, Unsigned8* tabledata, Unsigned16 tabledata_length);
Boolean C1222Client_Send_Logon_Request(C1222Client * pClient, Unsigned8 * User_Info, Unsigned8 * User_id, Unsigned16 session_idle_timeout);
Boolean C1222Client_Send_Logoff_Request(C1222Client * pClient);
Boolean C1222Client_Send_Terminate_Request(C1222Client * pClient);
Boolean C1222Client_Send_Disconnect_Request(C1222Client * pClient);
Boolean C1222Client_Send_Wait_Request(C1222Client * pClient, Unsigned8 duration);
Boolean C1222Client_Send_Resolve_Request(C1222Client * pClient, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length);
Boolean C1222Client_Send_Trace_Request(C1222Client * pClient, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length);
Boolean C1222Client_Process_Received_Message(C1222Client * pClient, ACSE_Message* pReceived_Msg, C1222StandardVersion Version, Unsigned8 Expected_type_of_response);

