#include "KeepConnectionThread.h"
#include "ServerConnectionThread.h"

void* KeepConnectionThread(void* data)
{
	
	struct thread* new_data = data;
	char *online_clients = malloc(1024);
	int result_client = 0;

	while(1){
		sleep(120);					
		sendToServer(new_data->sock, "OK", 3);					
		recieveOnlineClients(new_data->sock, online_clients);
		result_client = scanDatabase(online_clients, new_data->client);
		if(result_client == 0){ // if the client is not in list of clients so drop the connection.
			printf("the client is not online anymore...\n");
			remove("online_clients.txt");
			break;
		}
	}
	return (NULL);
}
