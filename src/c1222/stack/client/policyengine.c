//#include "c1222al.h"
//#include "c1222server.h"
//#include "c1222response.h"
//#include "c1222misc.h"
//#include "c1222_client.h"
#include <math.h>
//#include <vector>
//#include <queue>
//#include <string>
//#include <dirent.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <cmath>
//#include <algorithm>
#include "policyengine.h"
#include <stdio.h> 
#include <process.h> 
#include <stdlib.h>
#include<winsock2.h>

int Hex2Int(char hex)
{
	if(hex >= '0' && hex <= '9')
		return hex-'0';
	else if(hex >= 'A' && hex <= 'Z')
		return 10+hex-'A';
	else
		return 10+hex-'a';
}


Boolean epsemControl_Checks(Unsigned8 epsemControl, Boolean * isEd_Class_included)
{
	//printf("P.E : EpsemControl = %02X\n", epsemControl);
	if((epsemControl & 0x80) != 0x80) // Bit 7 should always be set to 1.
		return FALSE;
	else
	{
		if((epsemControl & 0x03) == 0x02) // It is a one way message. Check if ed-class is included.
		{
			if((epsemControl & 0x10) != 0x10) // Ed-class not included.
			{	*isEd_Class_included = FALSE;
			//	return FALSE; // For now
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
		for(i=0; i< Service_length;){
			//Possible error responses are isss, bsy, err , ok
			if( i == 0)
			{
				if(buffer[index + i] == 0x0A || buffer[index + i] == 0x06 || buffer[index + i] == 0x01 || buffer[index + i] == 0x00) // Check error responses
				{
					if(buffer[index + i] != 0x00 ) // if it is not a ok response, then the next index should be the end of the message
					{
						if( Service_length != 1) // Length of message should be only 1.
							return FALSE;
						else
							return TRUE;
					}
					i = i + 1;
				}
				else{
					return FALSE; // this is not a known reponse for Ident.
				}
			}
			else if( i == 1) // Check the protocol type. It should be c12.22
			{
				if(buffer[index + i] != 0x03)
					return FALSE;
				i = i + 1;
			}
			else if( i == 2) // Check the version.
			{
				if(buffer[index + i] != 0x01) // Not sure about this.
					return FALSE;
				i = i + 1;
			}
			else if ( i== 3) // Check the revision.
			{
				if(buffer[index + i] != 0x00) // Not sure about this.
					return FALSE;
				i = i + 1;
			}
			else
			{
				if(buffer[index + i] == 0x00) // End of feature_list
					break;
				else if(buffer[index +i] == 0x07) // Device Identity
				{
					Unsigned8 Identity_len = 0;
					Identity_len = buffer[index + i + 1] ;
					i = i + Identity_len + 2; // Skip over the Device Identity Info
				}
				else if(buffer[index + i] == 0x05) // Session control
				{
					i = i + 2;
				}
				else if(buffer[index + i] == 0x04) //Security Mechanism
				{
					Unsigned8 Uid_element_length;
					Uid_element_length = buffer[index + i + 2];
					i = i + (Unsigned8)Uid_element_length + 3; // Skip this field.
				}
				else if(buffer[index + i] == 0x06) //Device class element
				{
					Unsigned8 Uid_element_length;
					Uid_element_length = buffer[index + i + 2];
					i = i + Uid_element_length + 3; // Skip this element
				}
				else{ // This is not an indicator of a valid feature list.
					printf("Value of buffer at this index is %d, Index = %d\n", buffer[index + i],index + i);
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
Unsigned8 count, CRC;
Unsigned16 temp1, temp2;

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
			
			temp1 = buffer[index + 1];
			temp2 = buffer[index + 2];
			count = (temp1 & 0xFF00) | (temp2 & 0x00FF);
			CRC = ComputeCRC(&buffer[index +3],(Unsigned16)count);
			if(CRC != buffer[index + 3 + count ]) // Checksum failed.
			{
			
				printf("P.E : Check sum failed in Epsem message\n");
				printf("P.E Service_length = %d, Count = %d, Computed CRC = %02X, CRC in message = %02X\n", Service_length, count,CRC,buffer[index + 3 + count]); 
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
	if(Service_length > 1) // It should be just ok or nok. Length should be just 1
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
		if(Service_length > 1) // It should be just nok. Length should be just 1
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
	if(Service_length > 1) // It should be just ok or nok. Length should be just 1
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
	if(Service_length > 1) // It should be just ok or nok. Length should be just 1
		return FALSE;
	if(buffer[index] & 0xF0 != 0x00 ) // Error codes only range from 0x00 to 0x0F
		return FALSE;
	return TRUE;					
}

Boolean Validate_Disconnect_Response(Unsigned8* buffer, Unsigned16 length, Boolean isEd_Class_included)
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
	if(Service_length > 1) // It should be just ok or nok. Length should be just 1
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
	if(Service_length > 1) // It should be just response code. Length should be just 1
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
		if(Service_length > 1) // It's a uat error. Length should be just 1
			return FALSE;
	}
	else{
		if(buffer[index] != 0x00 ) // only other possible error code is OK.
			return FALSE;
		else
		{
			Unsigned8 local_addr_length = 0;
			local_addr_length = buffer[index + 1];
			if(Service_length != local_addr_length + 2) //Check bounds
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
	while(Service_length != 0)
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
	printf("Reg : Msg format is of type CSV2006. It is not supported\n");
	return FALSE; // For now.
}	
else{
	if(!ACSE_Message_IsValidFormat2008(pReceived_Msg)){
		printf("Reg : ACSE Message is not in valid 2008 format\n");
		return FALSE;
	}
	else
	{
		if(!ACSE_Message_GetApData( pReceived_Msg, &buffer, &length) ){
			printf("Reg : Get Ap Data failed\n");
			return FALSE;
		}
		else{
			epsemControl = buffer[0];
			if(!epsemControl_Checks(epsemControl,&isEd_Class_included)){
				printf("Reg : EpsemControl Checks Failed\n");
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

// Entropy checks

double entropy(int counts[], int arraysize, int dataSize) {
   double entropy = 0;
   double p = 0;
   int i = 0;
   
   if (dataSize == 0) {
     return 0;
   }
   
   for (i=0; i<arraysize; i++) {
	
     if (counts[i] == 0) {
       continue; // otherwise we get a NaN
     }
     p = (double)counts[i] / (double)dataSize;
	 if(p!= 0.0)
		entropy += (-1) * p * (log(p)/log(2));
     
   }

   return entropy;
 }
void EntropyAnalysis(Unsigned8 * buff, Unsigned16 length)
{
	
	int count = 0;
	const double thres = 6.67;
	const int BLOCKSIZE = 100;
	int array[256]={0};
	int hit = 0, miss = 0, j =0 ,i = 0,k = 0;
	
	for(j=0; j<=length/BLOCKSIZE; j++)
	{
		for(i=j*BLOCKSIZE;i<min((j+1)*BLOCKSIZE,length);i++)
		{
			int idx = (unsigned char)buff[i];
			array[idx]++;
			count++;
		}

		if(entropy(array,256, count)>thres)
			hit++;
		else
			miss++;
		
		printf("P.E : Entropy of Block %d is %f\n", j, entropy(array,256, count));
		for(k = 0 ; k < 256; k ++)
			array[k] = 0;
		//std::fill(array, array+256, 0);
		count = 0;
	}
		

	
	if(hit + miss > 0)
		printf("P.E : performed entropy analysis. %f percent of %d blocks are encrypted\n",(double)(hit*100)/(hit+miss),(hit+miss));
	
	
	return ;
}


void OneGramMeterbits(Unsigned8* buffer, Unsigned16 length, int offset, int* no_of_hits, int* no_of_misses){
	
	const int DIST = 4;
	const double thres = 0.2;
	const int BLOCKSIZE = 12800;
	int array[256];
	int size = 0, idx = 0, i =0;
	int count = 0, startIndex = offset,counter = 0;
	for(i = 0 ; i < 256; i ++)
		array[i] = 0;
	i = 0;
	//vector<int> previdx;
	
	if(length < BLOCKSIZE)
	{
		while(startIndex + size < length)
		{
		
			if(size%DIST == 3)
			{
				idx = (int)(buffer[startIndex + size]);
				array[idx]++;
				count++;
			}
		
			size++;
		}
	
		if( count > 0 && ((double)array[14]/count)>thres) // this is the e-test
			*no_of_hits = *no_of_hits + 1;
		else
			*no_of_misses = *no_of_misses + 1;
			
		if(*no_of_hits + *no_of_misses > 0)
			printf("P.E : performed the e-code test for offset %d.Percentage of hits recorded = %f\n",offset,(double)((*no_of_hits) *100)/(*no_of_hits + *no_of_misses));
	
	
	}
	else{ // there are multiple blocks
		while(startIndex + size < length)
		{
			counter ++;
			if(size%DIST == 3)
			{
				idx = (int)buffer[startIndex + size];
				array[idx]++;
				count++;
			}
			if(size%BLOCKSIZE == 0)
			{
				if(count > 0 && ((double)array[14]/count)>thres) // this is the e-test
					*no_of_hits++;
				else if(count > 0)
					*no_of_misses++;
				array[14]=0;
				count = 0;
				counter = 0;
			}
			size++;
		}

		if(counter < BLOCKSIZE && counter > 0) // the last part of the message which may not fit into a block
		{
			if(count > 0 && ((double)array[14]/count)>thres) // this is the e-test
				*no_of_hits = *no_of_hits + 1;
			else if(count > 0)
				*no_of_misses = *no_of_misses + 1;
		}
		if(*no_of_hits + *no_of_misses > 0)
			printf("P.E : performed the e-code test for offset %d.Percentage of hits recorded = %f\n",offset,(double)((*no_of_hits) *100)/(*no_of_hits + *no_of_misses));
	
	}
	
	
	

	
	
}
void bitsAnalysis(Unsigned8* buffer, Unsigned16 length)
{      
	int no_of_hits = 0, no_of_misses = 0;
	int offset = 0, max_offset = length -4;
	
	
	for(offset = 0; offset < max_offset; offset++){
		no_of_hits = 0, no_of_misses = 0;
		//vector<double> byteFreq(256,0);
		OneGramMeterbits(buffer,length,offset,&no_of_hits,&no_of_misses);
		
	}
}
int x86_code_test(char * received_msg)
{
	WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    char *message;
	char server_reply[100];
	int recv_size,result;
 
    printf("\nP.E : Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("P.E: Failed. Error Code : %d",WSAGetLastError());
        return -1;
    }
     
    printf("Initialised.\n");
     
    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("P.E : Could not create socket : %d" , WSAGetLastError());
		return -1;
    }
 
    printf("P.E : Socket created.\n");
     
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(2700);
 
    //Connect to remote server
    if (connect(s,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("P.E : connect error");
        return -1;
    }
     
    puts("P.E : Connected to x86 detector process");
     
    //Send some data
    message = received_msg;
    if( send(s ,message, strlen(message), 0) < 0)
    {
        puts("P.E : Send failed");
        return -1;
    }
    puts("P.E : Data Sent to x86 detector process\n");
	
	//Receive a reply from the server
    if((recv_size = recv(s , server_reply , 100 , 0)) == SOCKET_ERROR)
    {
        puts("P.E : Recv failed");
		return -1;
    }
     
    
 
    //Add a NULL terminating character to make it a proper string before printing
    server_reply[recv_size] = '\0';
	printf("P.E : Reply received from x86 detector is %s\n",server_reply);
	
	if(strcmp(server_reply,"It does not contain x86 executable content") == 0){
		return 1;
	
	}
	else 
		return 0;
    //result = atoi(server_reply);
	//if(result == 1){
	//	printf("Failed the x86 test\n");
	//}
	//else
	//{
	//	printf("passed x86 test\n");
	//}
	//return 0;


}
Boolean C1222PolicyEngine_Process_Received_Message(C1222Client * pClient, ACSE_Message* pReceived_Msg, C1222StandardVersion Version, Unsigned8 type_of_message)
{

	Unsigned8 * Epsem_body;
	Unsigned16 Epsem_length;
	Unsigned8 predicted_label;
	int result;
	char received_msg_length[5];
	char * received_msg;
	int i;
	pClient->responseMsgbuffer = pReceived_Msg->buffer;
	
	
	if(!Check_Received_Message_Semantics(pReceived_Msg,Version, type_of_message))
	{
		printf("P.E : Semantics check failed in the Received Message\n");
		return FALSE;
	}
	else{
		if(!ACSE_Message_GetApData( pReceived_Msg, &Epsem_body, &Epsem_length) ){
			printf("P.E : Get Application Data failed in the Received Message\n");
			return FALSE;
		}
		
	}
	printf("P.E : Success! Message Passed semantics check\n");
	
	//perform entropy analysis
	
	EntropyAnalysis(pReceived_Msg->buffer, pReceived_Msg->length);
	
	//perform ARM-code test
	
	bitsAnalysis(pReceived_Msg->buffer, pReceived_Msg->length);
	
	//perform x86-code test
	printf("P.E : Performing x86-code test ...\n");
	received_msg = (unsigned char *)malloc(2*pReceived_Msg->length*sizeof(unsigned char));
	for(i = 0 ; i < pReceived_Msg->length; i++)
			snprintf(received_msg + 2*i, 2,"%02x",pReceived_Msg->buffer[i]);
			
	snprintf(received_msg_length,5,"%d",2*pReceived_Msg->length);
	//predicted_label = spawnl( P_WAIT, "x86_detection_experiment.exe",
    //"x86_detection_experiment.exe", "4", "C:/Python27/x86_detection/classifier", (unsigned char *)received_msg, received_msg_length, NULL );
	//if(predicted_label == 1)
	//	printf("Failed the x86 code test\n");
	//else 
	
	result = x86_code_test(received_msg);
	free(received_msg);
	if(result == 1 ){
		printf( "P.E : Passed x86 test \n");
	}
	else if(result == 0 ){
		printf("P.E : Failed  x86 test \n");
		return FALSE;
	}
	else if(result == -1){
		printf("P.E : Encountered error. Could not perform x86 test \n");
		return FALSE;
	}
	printf("P.E : Performed all the policy checks\n");
	
	return TRUE;
}