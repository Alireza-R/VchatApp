#include "ServerConnectionThread.h"

#define SIZE (30) 
void* ServerConnectionThread(void* data)
{

	struct thread* new_data = data;
	char* message = malloc(SIZE);
	char* client = malloc(SIZE);
	char *online_clients = malloc(1024);
	int result_client = 0;

	
	recieveOnlineClients(new_data->sock, online_clients);	// choosing the client from the list that he wishes to chat	
	printf("Please enter the name of online client which you want to chat: \n");
	scanf("%s", client);
	result_client = scanDatabase(online_clients, client); // scan the databse to see if this user is online

	if(result_client == 0)
		printf("the name doesn't exits\n");
	else{
		sendToServer(new_data->sock, client, SIZE);
		bzero(message, SIZE);
		recieveFromServer(new_data->sock, message, SIZE);	
		printf("IP address of other client is: %s\n", message);	
		strcpy(new_data->IPaddr, message);
		strcpy(new_data->client, client);
	}

	return (NULL);
}
