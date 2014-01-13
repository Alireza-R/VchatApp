#include "ClientThread.h"

#define SIZE (30)
#define STIME (120)


void* ClientThread(void* sock)
{
	int new_sock = *((int*)sock);
	free(sock);
	pthread_detach(pthread_self());
	char *message = malloc(SIZE);
	int result = 0;
	
	bzero(message, SIZE);
	recieveFromClient(new_sock, message, 7);	
	
	if(strcmp(message, "signup") == 0){       //sign up request
			signupClient(new_sock);
			sendToClient(new_sock, "signupok", 9); //sign up was successful
	}
	else if(strcmp(message, "signin") == 0){   //sign in request
			result = signinClient(new_sock);
			if (result != 0){
				sendToClient(new_sock, "signinok", 9); //sign in was successful
				bzero(message, SIZE);				
				recieveFromClient(new_sock, message, 3);
				if(strcmp(message, "NO") != 0){		
					sendOnlineClients(new_sock); //send the list of online clients
					bzero(message, SIZE);					
					recieveFromClient(new_sock, message, SIZE);		
					scanOnlineClients(message);
					sendToClient(new_sock, message, SIZE);
					while(1){
						sleep(STIME); // send periodic updates every 5 minutes
						bzero(message, SIZE);				
						recieveFromClient(new_sock, message, SIZE);
						if(strcmp(message, "OK") == 0)
							sendOnlineClients(new_sock); // for synchonization 
						else{
							removeClient(new_sock);
							break;
						}	
					}
				}
			}
			else
				sendToClient(new_sock, "wronginp", 9); //sign in failed. try again.
	}
	else
			printf("No input\n"); //Not acceptable request



keepConnection:	while(1){
			sleep(STIME); // send periodic updates every 5 minutes
			bzero(message, SIZE);				
			recieveFromClient(new_sock, message, SIZE);
			if(strcmp(message, "OK") == 0)
				sendOnlineClients(new_sock); // for synchonization 
			else{
				removeClient(new_sock);
				break;
			}	
		}
	
	removeClient(new_sock);
	close(new_sock);

	return (NULL);
}
