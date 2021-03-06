// This is the client program.
// Author - Vignesh

#include "c1222al.h"
#include "c1222server.h"
#include "c1222response.h"
#include "c1222misc.h"
#include "c12222_client.h"


void setmyApTitle(C1222Client * pClient, Unsigned8* Aptitle, Unsigned8 length)
{
pClient->myAptitle_length = length;
pClient->myAptitle = Aptitle;
}

void setmyMRApTitle(C1222Client * pClient, Unsigned8* Aptitle, Unsigned8 length)
{
pClient->myMRAptitle_length = length;
pClient->myMRAptitle = Aptitle;
}

void setCalled_Aptitle(C1222Client * pClient, Unsigned8 * Called_Aptitle, Unsigned8 Called_Aptitle_length)
{
pClient->Called_Aptitle = Called_Aptitle;
pClient->Called_Aptitle_length = Called_Aptitle_length;
}

void setDestination_Native_Address(C1222Client * pClient, Unsigned8 * Destination_Native_Address, Unsigned16 Destination_Native_Address_length)
{
pClient->Destination_Native_Address = Destination_Native_Address;
pClient->Destination_Native_Address_length = Destination_Native_Address_length;
}

void setCallingApInvocationId(C1222Client * pClient, Unsigned16 CallingAp_InvocationId)
{
pClient->CallingAp_InvocationId = CallingAp_InvocationId;
}
// Request for Identity Information
Unsigned16 ComposeIdentRequest_Epsem(Unsigned8* request)
{
    // I guess we will respond with 2008 format until told otherwise - I think only the collection engine will ask and we should
    // respond in 2008 format for it.
    
    Unsigned16 request_length = 0;
    Unsigned8 * init_pos = request;
	request++;
    
	
	*request++ = 0x20;      // Code for Ident Request
	request_length++;
    
	*init_pos = (request_length &0xFF);
    
    return request_length + 1;
} 

//Read from Table request
Unsigned16 ComposeReadRequest_Epsem(Unsigned8* request, Unsigned8* tableId, Boolean isFullRead, Unsigned8* offset, Unsigned8* octet_count)
{
    // I guess we will respond with 2008 format until told otherwise - I think only the collection engine will ask and we should
    // respond in 2008 format for it.
    
    Unsigned16 request_length = 0;
	Unsigned8 * init_pos = request;
	request++;
	
    if(isFullRead){
		*request++ = 0x30;      //Code for Read request
		request_length++;
		memcpy(request, tableId, 2); // Copy table ID. Its length is 2
		request += 2;
		request_length += 2;
	}
	else{
		*request++ = 0x3F; //Code for offset based read. Index based read not supported
		request_length++;
		memcpy(request, tableId, 2); // Copy table ID
		request += 2;
		request_length += 2;
		*request++ = offset[0];
		*request++ = offset[1];
		*request++ = offset[2]; //Offset is a 24 bit word
		request_length += 3;
		*request++ = octet_count[0];
		*request++ = octet_count[1]; // octet count is a 16bit word
		request_length +=2;
	}
	
    *init_pos = (request_length &0xFF);
    return request_length + 1;
} 

//This function computes the 8 bit checksum.
Unsigned8 ComputeCRC(Unsigned8 * data, Unsigned16 size)
{
Unsigned8 checksum;
Unsigned16 i = 0;
for(i = 0; i<size;i++){
checksum += data[i]

}
checksum &= 0xFF;
return checksum;

}

// Write to Table Request
Unsigned16 ComposeWriteRequest_Epsem(Unsigned8* request, Unsigned8* tableId, Boolean isFullWrite, Unsigned8* offset, Unsigned8* tabledata, Unsigned16 tabledata_length)
{
    // I guess we will respond with 2008 format until told otherwise - I think only the collection engine will ask and we should
    // respond in 2008 format for it.
    
    Unsigned16 request_length = 0;
	Unsigned8 checksum;
	Unsigned16 i = 0;
	
	Unsigned8 * init_pos = request;
	request++;
	
	checksum = ComputeCRC(tabledata, tabledata_length);
	
    if(isFullWrite){
		*request++ = 0x40;      //Code for full write request
		request_length++;
		memcpy(request, tableId, 2); // Copy table ID
		request += 2;
		request_length += 2;
		
		// Start to copy the table data <count><data><checksum>
		*request++ = ( tabledata_length >> 8); // Copy the count
		*request++ = ( tabledata_length & 0x00FF);
		request_length += 2;
		for( i =0; i< tabledata_length; i++){ // Copy the table data
			*request++ = tabledata[i];
			request_length ++;
		}
		*request++ = checksum
		request_length ++;
	}
	else{
		*request++ = 0x4F; //Code for offset based Write. Index based Write not supported
		request_length++;
		memcpy(request, tableId, 2); // Copy table ID
		request += 2;
		request_length += 2;
		*request++ = offset[0]
		*request++ = offset[1]
		*request++ = offset[2] //Offset is a 24 bit word
		request_length += 3
		// Start to copy the table data <count><data><checksum>
		*request++ = ( tabledata_length >> 8); // Copy the count
		*request++ = ( tabledata_length & 0x00FF);
		request_length += 2;
		for( i =0; i< tabledata_length; i++){ // Copy the table data
			*request++ = tabledata[i];
			request_length ++;
		}
		*request++ = checksum
		request_length ++;
	}
	
    *init_pos = (request_length &0xFF);
    return request_length + 1;
} 


//Log on service request
Unsigned16 ComposeLogonServiceRequest_Epsem(Unsigned8* request, Unsigned8 * User_Info, Unsigned8 * User_id, Unsigned16 session_idle_timeout)
{
Unsigned16 request_length =0, i;
Unsigned8 * init_pos = request;
request++;

*request++ = 0x50; // Code for logon request
*request++ = User_id[0];
*request++ = User_id[1];
request_length += 3;

//Copy 10 bytes containing user information
for(i=0;i<10;i++)
{
	*request++ = User_Info[i];
	request_length ++;
}

*request++ = (session_idle_timeout >> 8); //Copy the idle timeout for the session
*request++ = (session_idle_timeout & 0xFF);
request_length +=2;

*init_pos = (request_length &0xFF);
return request_length
}

// Log off Service request
Unsigned16 ComposeLogoffServiceRequest_Epsem(Unsigned8* request)
{

Unsigned16 request_length = 0;
Unsigned8 * init_pos = request;
request++;

*request++ = 0x52;
request_length ++;

*init_pos = (request_length &0xFF);
return request_length + 1;

}

//Terminate Service Request
Unsigned16 ComposeTerminateServiceRequest_Epsem(Unsigned8* request)
{

Unsigned16 request_length = 0;
Unsigned8 * init_pos = request;
request++;

*request ++ = 0x21;
request_length ++;

*init_pos = (request_length &0xFF);
return request_length + 1;

}

// Disconnect Service Request
Unsigned16 ComposeDisconnectServiceRequest_Epsem(Unsigned8* request)
{

Unsigned16 request_length = 0;
Unsigned8 * init_pos = request;
request++;

*request ++ = 0x22;
request_length ++;

*init_pos = (request_length &0xFF);
return request_length + 1;

}


// Wait Service Request
Unsigned16 ComposeWaitServiceRequest_Epsem(Unsigned8* request, Unsigned8 duration)
{

Unsigned16 request_length = 0;
Unsigned8 * init_pos = request;
request++;

*request ++ = 0x70;
request_length ++;
*request++ = duration;
request_length++;

*init_pos = (request_length &0xFF);
return request_length + 1;

}

// Resolve service Request
Unsigned16 ComposeResolveServiceRequest_Epsem(Unsigned8* request, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length)
{

Unsigned16 request_length = 0;
Unsigned8 * init_pos = request;
request++;

*request ++ = 0x25;
request_length ++;
memcpy(request, targetNodeApTitle, Aptitle_length);
*request += Aptitle_length;
request_length += Aptitle_length;

*init_pos = (request_length &0xFF);
return request_length + 1;

}

// Trace service Request
Unsigned16 ComposeTraceServiceRequest_Epsem(Unsigned8* request, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length)
{

Unsigned16 request_length = 0;
Unsigned8 * init_pos = request;
request++;

*request ++ = 0x26;
request_length ++;
memcpy(request, targetNodeApTitle, Aptitle_length);
*request += Aptitle_length;
request_length += Aptitle_length;

*init_pos = (request_length &0xFF);
return request_length + 1;

}

//TODO : Register service request.



//Create a common header for all types of requests. Fields obtained from simulatehost.c
//Calling Ap invocation id is an argument should be less than 128.
Unsigned16 Create_ACSE_Header(ACSE_Message* pRequestMsg, Unsigned8* Called_Aptitle, Unsigned8 length_of_called_Aptitle, Unsigned8* Calling_Aptitle, Unsigned8 length_of_callng_Aptitle, Unsigned16 CallingAp_InvocationId, C1222StandardVersion standardVersion) 
{
//C1222ApTitle apTitle;
    static Unsigned8 localBuffer[MAX_ACSE_HDR_MSG_SIZE];   
    
    if (standardVersion == CSV_MARCH2006 )
    {
        // Don't know how to construct a response. I think it can be ignored because it is an old standard anyway.
    }
    else
    {
        localBuffer[0] = 0x60; //ACSE identifier
        localBuffer[1] = 16 + length_of_called_Aptitle + length_of_callng_Aptitle; // length of whole ACSE message        
        localBuffer[2] = 0xA2; // Called Aptitle element or destination*
        localBuffer[3] = length_of_called_Aptitle; // Called Aptitle length
        memcpy(&localBuffer[4], Called_Aptitle, length_of_called_Aptitle);
        if ( myNodeApTitle[0] == 0x0d )
            localBuffer[4] = 0x80;
            
        localBuffer[4 + length_of_called_Aptitle] = 0xA4;//called-AP-invocation-id-element
        localBuffer[5 + length_of_called_Aptitle] = 4;//called-AP-invocation-id-integerlength
        localBuffer[6 + length_of_called_Aptitle] = 0x02;//called-AP-invocation-id-integer
        localBuffer[7 + length_of_called_Aptitle] = 2;//called-AP-invocation-id-length
        localBuffer[8 + length_of_called_Aptitle] = (Unsigned16)(sequence >> 8);//called-AP-invocation-id
        localBuffer[9 + length_of_called_Aptitle] = (Unsigned16)(sequence & 0xFF); // need a called ap invocation id equal to calling from msg //called-AP-invocation-id

        localBuffer[10 + length_of_called_Aptitle] = 0xA6; // Calling Aptitle element or initiator*
        localBuffer[11 + length_of_called_Aptitle] = length_of_callng_Aptitle; // Calling Aptitle length
        memcpy(&localBuffer[12 + length_of_called_Aptitle], Calling_Aptitle, length_of_callng_Aptitle);
        if ( myMRApTitle[0] == 0x0d )
            localBuffer[12 + length_of_called_Aptitle] = 0x80;
            
        localBuffer[12 + length_of_called_Aptitle + length_of_callng_Aptitle] = 0xA8;//calling-AP-invocation-id-element
        localBuffer[13 + length_of_called_Aptitle + length_of_callng_Aptitle] = 3; // Its length
        localBuffer[14 + length_of_called_Aptitle + length_of_callng_Aptitle] = 0x02; //calling-AP-invocation-id-integer
        localBuffer[15 + length_of_called_Aptitle + length_of_callng_Aptitle] = 2; //calling-AP-invocation-id-length
        localBuffer[16 + length_of_called_Aptitle + length_of_callng_Aptitle] = (CallingAp_InvocationId >> 8); //calling-AP-invocation-id
        localBuffer[17 + length_of_called_Aptitle + length_of_callng_Aptitle] = (CallingAp_InvocationId & 0xFF); //calling-AP-invocation-id
        
        
        pRequestMsg->buffer = &localBuffer[2];
        pRequestMsg->length = 17 + length_of_called_Aptitle + length_of_callng_Aptitle;
        pRequestMsg->maxLength = MAX_ACSE_MSG_SIZE; // Dont know. have to check it later
    }

    

}

}

Boolean CreateRequestMessage_Ident(ACSE_Message * pRequestMsg)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeIdentRequest_Epsem(Msg_body);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}

Boolean CreateRequestMessage_Read(ACSE_Message * pRequestMsg, Unsigned8* tableId, Boolean isFullRead, Unsigned8* offset, Unsigned8* octet_count)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeReadRequest_Epsem(Msg_body, tableId, isFullRead, offset, octet_count);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}

Boolean CreateRequestMessage_Write(ACSE_Message * pRequestMsg, Unsigned8* tableId, Boolean isFullWrite, Unsigned8* offset, Unsigned8* tabledata, Unsigned16 tabledata_length)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeWriteRequest_Epsem(Msg_body, tableId, isFullWrite, offset, tabledata, tabledata_length);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}


Boolean CreateRequestMessage_Logon(ACSE_Message * pRequestMsg, Unsigned8 * User_Info, Unsigned8 * User_id, Unsigned16 session_idle_timeout)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeLogonRequest_Epsem(Msg_body, User_Info, User_id, session_idle_timeout);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}


Boolean CreateRequestMessage_Logoff(ACSE_Message * pRequestMsg)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeLogoffRequest_Epsem(Msg_body);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}


Boolean CreateRequestMessage_Terminate(ACSE_Message * pRequestMsg)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeTerminateRequest_Epsem(Msg_body);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}


Boolean CreateRequestMessage_Disconnect(ACSE_Message * pRequestMsg)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeDisconnectRequest_Epsem(Msg_body);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}


Boolean CreateRequestMessage_Wait(ACSE_Message * pRequestMsg,Unsigned8 duration)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeWaitRequest_Epsem(Msg_body, duration);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}


Boolean CreateRequestMessage_Resolve(ACSE_Message * pRequestMsg, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeResolveRequest_Epsem(Msg_body, targetNodeAptitle, Aptitle_length);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}

Boolean CreateRequestMessage_Trace(ACSE_Message * pRequestMsg, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length)
{
Epsem* EpsemMsg;
Unsigned8 Msg_body[MAX_EPSEM_BODY_SIZE];
Unsigned16 Msg_length;
EpsemMsg->epsemControl = 0x80; // The default control pattern.
Msg_length = ComposeTraceRequest_Epsem(Msg_body, targetNodeAptitle, Aptitle_length);
EpsemMsg->length = Msg_length - 1;
EpsemMsg->maxLength = Msg_length;
EpsemMsg->buffer = Msg_body
memset(EpsemMsg->deviceClass, 0, 4);
 if ( Epsem_Validate(&epsem) )
        {
            ACSE_Message_SetUserInformation(pRequestMsg, EpsemMsg, 0)
			return TRUE;
            
        } // end if the epsem validated
        else
        {
            printf("Failure in Epsem Validation\n");
			return FALSE;
            
        }

}


Boolean epsemControl_Checks(Unsigned8 epsemControl, Boolean * isEd_Class_included)
{
	if((epsemControl & 0x80) != 0x80) // Bit 7 should always be set to 1.
		return FALSE;
	else
	{
		if((epsemControl & 0x03) == 0x02) // It is a one way message. Check if ed-class is included.
		{
			if((epsemControl & 0x10) != 0x10) // Ed-class not included.
			{	*isEd_Class_included = FALSE;
				return FALSE;
			}
			else
				*isEd_Class_included = TRUE;
		}
		else if((epsemControl & 0x03) == 0x03) // Not supported
				return FALSE;
	}
	return TRUE;
}


Boolean Validate_Ident_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{
Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	while(Service_length != 0) // We will only send one request at a time. So the reponse should contain only one epsem-data.( the while loop
								// should be executed only once.
	{
		for(i=0; i< Service_length; i++){
			//Possible error responses are isss, bsy, err , ok
			if( i == 0)
			{
				if(buffer[index + i] == 0x0A || buffer[index + i] == 0x06 || buffer[index + i] = 0x01 || buffer[index + i] == 0x00) // Check error responses
				{
					if(buffer[index + i] != 0x00 ) // if it is not a ok response, then the next index should be the end of the message
					{
						if( Service_length != 1) // Length of message should be only 1.
							return FALSE;
					}
					
				}
				else{
					return FALSE; // this is not a known reponse for Ident.
				}
			}
			else if( i == 1) // Check the protocol type. It should be c12.22
			{
				if(buffer[index + i] != 0x03)
					return FALSE;
			}
			else if( i == 2) // Check the version.
			{
				if(buffer[index + i] != 0x01) // Not sure about this.
					return FALSE;
			}
			else if ( i== 3) // Check the revision.
			{
				if(buffer[index + i] != 0x00) // Not sure about this.
					return FALSE;
			}
			else
			{
				if(buffer[index + i] == 0x00) // End of feature_list
					break;
				else if(buffer[index +i] == 0x07) // Device Identity
				{
					Unsigned8 Identity_len = 0;
					Identity_len = buffer[index + i + 1] ;
					i = i + Identity_len; // Skip over the Device Identity Info
				}
				else if(buffer[index + i] == 0x05) // Session control
				{
					i = i + 2;
				}
				else if(buffer[index + i] == 0x04) //Security Mechanism
				{
					Unsigned8 Uid_element_length;
					Uid_element_length = buffer[index + i + 2];
					i = i + Uid_element_length + 3; // Skip this field.
				}
				else if(buffer[index + i] == 0x06) //Device class element
				{
					Unsigned8 Uid_element_length;
					Uid_element_length = buffer[index + i + 2];
					i = i + Uid_element_length + 3; // Skip this element
				}
				else{ // This is not an indicator of a valid feature list.
					printf("Not a valid feature list error\n");
					return FALSE;
				}
			}
			
			if( i > Service_length)
			{
				printf("Indicator of some invalid length fields in Epsem message\n");
				return FALSE;
			}
		
		}
	
		//index = index + Service_length + 1;
		//if(index <= length)
		//	Service_length = buffer[index];
		//else
		//	break;
		break;
	}
	return TRUE;

}

Boolean Validate_Read_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	while (Service_length != 0)
	{
		if(buffer[index] != 0x00) // Not an ok response. this should be the end of the message.
		{
			if(Service_length != 1)
				return FALSE;
			else;
				break;
		}
		else{
			Unsigned8 count, CRC;
			count = buffer[index + 1];
			CRC = ComputeCRC(&buffer[index +2],(Unsigned16)count);
			if(CRC != buffer[index +1 + count +1]) // Checksum failed.
			{
				printf("Check sum failed in Epsem message\n");
				return FALSE;
			}
			break;
		}
		
	
	}
	
	return TRUE;					
}


Boolean Validate_Write_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	if(Service_lengh > 1) // It should be just ok or nok. Length should be just 1
		return FALSE;
	if(buffer[index] & 0xF0 != 0x00 ) // Error codes only range from 0x00 to 0x0F
		return FALSE;
	return TRUE;					
}

Boolean Validate_Logon_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	if(buffer[index] == 0x00) // Response ok
	{
		if(Service_length > 3) // Response ok is followed only by a two byte message.
			return FALSE; 
		
	}
	else{
		if(Service_lengh > 1) // It should be just nok. Length should be just 1
			return FALSE;
		if(buffer[index] & 0xF0 != 0x00 ) // Error codes only range from 0x00 to 0x0F
			return FALSE;
	}
	return TRUE;					
}

Boolean Validate_Logoff_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	if(Service_lengh > 1) // It should be just ok or nok. Length should be just 1
		return FALSE;
	if(buffer[index] & 0xF0 != 0x00 ) // Error codes only range from 0x00 to 0x0F
		return FALSE;
	return TRUE;					
}

Boolean Validate_Terminate_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	if(Service_lengh > 1) // It should be just ok or nok. Length should be just 1
		return FALSE;
	if(buffer[index] & 0xF0 != 0x00 ) // Error codes only range from 0x00 to 0x0F
		return FALSE;
	return TRUE;					
}

Boolean Validate_Disconnect_Response_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	if(Service_lengh > 1) // It should be just ok or nok. Length should be just 1
		return FALSE;
	if(buffer[index] & 0xF0 != 0x00 ) // Error codes only range from 0x00 to 0x0F
		return FALSE;
	return TRUE;					
}

Boolean Validate_Wait_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	if(Service_lengh > 1) // It should be just response code. Length should be just 1
		return FALSE;
	if(buffer[index] == 0x00 || buffer[index] == 0x0A || buffer[index] == 0x06 || buffer[index] == 0x01 ) // Only ok, isss, bsy, err codes expected.
		return TRUE;
	return FALSE;					
}

Boolean Validate_Resolve_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;

	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	if(buffer[index] == 0x0C){ //uat error response code
		if(Service_lengh > 1) // It's a uat error. Length should be just 1
			return FALSE;
	}
	else{
		if(buffer[index] != 0x00 ) // only other possible error code is OK.
			return FALSE;
		else
		{
			Unsigned8 local_addr_length = 0;
			local_addr_length = buffer[index + 1]
			if(Service_lengh != local_addr_length + 2) //Check bounds
				return FALSE;
		}
	
	}
	return TRUE;					
}

Boolean Validate_Trace_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
{

Unsigned8 Start_Index = 0, index = 0;
Unsigned8 Service_length = 0,i = 0;
Unsigned8 total_Aptile_length = 0, Curr_Aptitle_length = 0, no_of_Aptitles = 0;
	if(isEd_Class_included)
		Start_Index = 5 ;// Ed_Class is 4 bytes + skipping first byte (epsemControl)
	else
		Start_Index = 1; // Skipping first byte (epsemControl)
	index = Start_Index;
	Service_length = buffer[index];
	index ++;
	
	if(buffer[index] & 0xF0 != 0x00 ) // Error codes only range from 0x00 to 0x0F
		return FALSE;
		
	index ++;
	
	While(Service_length != 0)
	{
		Curr_Aptitle_length = buffer[index];
		total_Aptile_length += Curr_Aptitle_length;
		no_of_Aptitles ++;
		if(total_Aptile_length + no_of_Aptitles +1 > Service_length ) // Not sure. Still in doubt.
			return FALSE;
		
	
	}
	return TRUE;					
}


// Based on the type of sent Message, check for syntax errors in the corresponding response.
Boolean Check_Received_Message_Semantics(ACSE_Message* pReceived_Msg, C1222StandardVersion standardVersion, Unsigned8 Type_of_sent_message)
{
Unsigned8 * buffer;
Unsigned16 length;
Unsigned8 epsemControl;
Boolean isEd_Class_included = FALSE;
if(standardVersion == CSV_MARCH2006){
	printf("Msg format is of type CSV2006. It is not supported\n");
	return FALSE; // For now.
}	
else{
	if(!ACSE_Message_IsValidFormat2008(pReceived_Msg)){
		printf("ACSE Message is not valid\n");
		return FALSE;
	}
	else
	{
		if(!ACSE_Message_GetApData( pReceived_Msg, &buffer, &length) ){
			printf("Get Ap Data failed\n");
			return FALSE;
		}
		else{
			if(!epsemControl_Checks(epsemControl,&isEd_Class_included)){
				printf("EpsemControl Checks Failed\n");
				return FALSE;
			}
			else{
			// Begin Validating the response.
				switch(Type_of_sent_message)
				{
				case REQUEST_TYPE_IDENT :	if(!Validate_Ident_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
				case REQUEST_TYPE_READ :	if(!Validate_Read_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
				case REQUEST_TYPE_WRITE :	if(!Validate_Write_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
				case REQUEST_TYPE_LOGON :	if(!Validate_Logon_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
											
				case REQUEST_TYPE_LOGOFF :	if(!Validate_Logoff_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
				case REQUEST_TYPE_TERMINATE :	if(!Validate_Terminate_Response(buffer,length,isEd_Class_included))
													return FALSE;
												break;
				case REQUEST_TYPE_DISCONNECT : 	if(!Validate_Disconnect_Response(buffer,length,isEd_Class_included))
													return FALSE;
												break;
				case REQUEST_TYPE_WAIT :	if(!Validate_Wait_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
				case REQUEST_TYPE_RESOLVE :	if(!Validate_Resolve_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
				case REQUEST_TYPE_TRACE : if(!Validate_Trace_Response(buffer,length,isEd_Class_included))
													return FALSE;
											break;
				default :	return FALSE;
							break;
				
				}
			}
		}
	}

}

return TRUE; // If it passes all checks.
}

C1222Client_Init(C1222Client * pClient, C1222Stack * pStack, Unsigned8 * requestMsgbuffer, Unsigned8* responseMsgbuffer)
{
pClient->pStack = pStack;
pClient->requestMsgbuffer = requestMsgbuffer;
pClient->responseMsgbuffer = responseMsgbuffer;

}

Boolean C1222Client_Send_Message(C1222Stack * pStack, ACSE_Message* Msg, Unsigned8* Destination_Native_Address, Unsigned16 Destination_Native_Address_length)
{
	return C1222AL_SendMessage(&pStack->Xapplication, Msg, Destination_Native_Address, Destination_Native_Address_length);
}


Boolean C1222Client_Send_Ident_Request(C1222 * pClient)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Ident(ACSE_Message * pRequestMsg))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}

pClient->requestMsgbuffer = pRequestMsg->buffer;

return TRUE;
}


Boolean C1222Client_Send_Read_Request(C1222 * pClient,Unsigned8* tableId, Boolean isFullRead, Unsigned8* offset, Unsigned8* octet_count)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Read(ACSE_Message * pRequestMsg, tableId, isFullRead, offset, octet_count))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}

pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Write_Request(C1222 * pClient,Unsigned8* tableId, Boolean isFullWrite, Unsigned8* offset, Unsigned8* tabledata, Unsigned16 tabledata_length)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Write(ACSE_Message * pRequestMsg, tableId, isFullWrite, offset, tabledata, tabledata_length))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}

pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Logon_Request(C1222 * pClient, Unsigned8 * User_Info, Unsigned8 * User_id, Unsigned16 session_idle_timeout)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Logon(ACSE_Message * pRequestMsg, User_Info, User_id, session_idle_timeout))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}

pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Logoff_Request(C1222 * pClient)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Logoff(ACSE_Message * pRequestMsg))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}
pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Terminate_Request(C1222 * pClient)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Terminate(ACSE_Message * pRequestMsg))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}
pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Disconnect_Request(C1222 * pClient)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Disconnect(ACSE_Message * pRequestMsg))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}
pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Wait_Request(C1222 * pClient, Unsigned8 duration)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Wait(ACSE_Message * pRequestMsg, Unsigned8 duration))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}
pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Resolve_Request(C1222 * pClient, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Resolve(ACSE_Message * pRequestMsg, targetNodeAptitle, Aptitle_length))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}
pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}


Boolean C1222Client_Send_Trace_Request(C1222 * pClient, Unsigned8* targetNodeAptitle, Unsigned8 Aptitle_length)
{
ACSE_Message pRequestMsg;
Create_ACSE_Header( &pRequestMsg, pClient->Called_Aptitle, pClient->Called_Aptitle_length_Aptitle, pClient->myAptitle, pClient->myAptitle_length, pClient->CallingAp_InvocationId, CSV_2008) 
if(!CreateRequestMessage_Trace(ACSE_Message * pRequestMsg, targetNodeAptitle, Aptitle_length))
	return FALSE;
else
{
		if(C1222Client_Send_Message(pClient->pStack, pRequestMsg, pClient->Destination_Native_Address, pClient->Destination_Native_Address_length) == FALSE)
			return FALSE;
}
pClient->requestMsgbuffer = pRequestMsg->buffer;
return TRUE;
}

Boolean C1222Client_Process_Received_Message(C1222 * pClient, ACSE_Message* pReceived_Msg, C1222StandardVersion Version, Unsigned8 Expected_type_of_response)
{

	Unsigned8 * Epsem_body;
	Unsigned16 Epsem_length;
	pClient->responseMsgbuffer = pReceived_Msg->buffer;
	
	if(!Check_Received_Message_Semantics(pReceived_Msg,Version, Expected_type_of_response))
	{
		printf("Semantics check failed in Process Message\n");
		return FALSE;
	}
	else{
		if(!ACSE_Message_GetApData( pReceived_Msg, &Epsem_body, &Epsem_length) ){
			printf("Get Ap Data failed in Process Message\n");
			return FALSE;
		}
		//perform other security checks here. (Entropy and ARM code checks). We now have the Received ACSE message as well as the Epsem body.
	}
	return TRUE;
}


