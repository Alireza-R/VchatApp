#include "Server.h"
#include "ClientThread.h"


int main()
{
	int sockPT; 
	int *next_sock;
	pthread_t sId1;

	
	createSocket(&sockPT);
	listen(sockPT,5);

	while(1){
		next_sock = malloc(sizeof(int));
		acceptNewClient(sockPT, next_sock);
		pthread_create(&sId1, NULL, &ClientThread, next_sock);
	}

	exit(0);
}
