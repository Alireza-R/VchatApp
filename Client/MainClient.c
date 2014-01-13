#include <pthread.h>
#include "Client.h"
#include "ServerConnectionThread.h"
#include "KeepConnectionThread.h"
#include "SendClientThread.h"
#include "RecieveClientThread.h"

#define SIZE (30)
#define RECVSENDPORT (30020)
#define SENDRECVPORT (30010)


int main()
{
	int sockPT, sockPT2, select;
	char* message = malloc(SIZE);
	pthread_t cId1, cId2, cId3, cId4;
	struct thread* data = malloc(sizeof(struct thread)); 
	struct sockaddr_in sockAddress;
	int addrlen = sizeof(struct sockaddr);
	struct signupData* info1 = calloc(1, sizeof(struct signupData));
	struct signinData* info2 = calloc(1, sizeof(struct signinData));

	printf("Please choose one of the options\n");
	printf("1: Sign Up to the server (new users)\n");
	printf("2: Sign In to the server (registered users)\n");
	scanf("%d", &select);
	switch(select){
		case 1: // the user wants to sign up (new users)
		signupInformation(&info1);
		createSocket(&sockPT);
		sendToServer(sockPT, "signup", 7); //sign up request
		signupToServer(sockPT, info1);
		break;

		case 2: // the user wants to sign in (registered users)
		signinInformation(&info2);
		createSocket(&sockPT);
		sendToServer(sockPT, "signin", 7); //sign in request
		signinToServer(sockPT, info2);
		break;

		default: // the input is not acceptable
		printf("Wrong input. Bye\n");
		exit(0);
	}
	bzero(message, 30);
	recieveFromServer(sockPT, message, 9); // recieve confirmatio from server if sign up/ sign in was successful
	if(strcmp(message, "signupok") == 0)    //sign up completed
		printf("Signed up is complete\n");

	else if(strcmp(message, "signinok") == 0){  //sign in completed
		printf("Signed in is complete\n");
		data->sock = sockPT;
		bzero(data->IPaddr, 0);
		printf("Please choose:\n");
		printf("1. Wait for a request\n");
		printf("2. Make a new voice chat request\n");
		scanf("%d", &select);
		if(select == 1){
			sendToServer(sockPT, "NO", 3);			
			recieveFromClient(sockPT2, data->IPaddr, SIZE);
			data->Port = RECVSENDPORT; // choose a different port number for sending data
			pthread_create(&cId3, NULL, &SendClientThread, data); // make udp connection to the other client (sending)
			sleep(8);
			data->Port = SENDRECVPORT; // choose a different port number for recieving data
			pthread_create(&cId4, NULL, &RecieveClientThread, data); // make udp connection to the other client (recieving)
			pthread_join(cId3, NULL);
			pthread_join(cId4, NULL);	
		  }
		else if(select == 2){
			sendToServer(sockPT, "OK", 3);
			pthread_create(&cId1, NULL, &ServerConnectionThread, data); // start a new thread for connection to the server
			pthread_join(cId1, 0); // wait to recieve the IP address of the other client
			getsockname(sockPT, (struct sockaddr*)&sockAddress, &addrlen);
			strcpy(message, (char*)(inet_ntoa(sockAddress.sin_addr)));
			sendToClient(sockPT2, message, data->IPaddr, SIZE);
			pthread_create(&cId2, NULL, &KeepConnectionThread, data); // recieve updates from server
			data->Port = RECVSENDPORT;
			pthread_create(&cId4, NULL, &RecieveClientThread, data); // make udp connection to the other client (recieving)
			sleep(8);
			data->Port = SENDRECVPORT;
			pthread_create(&cId3, NULL, &SendClientThread, data); // make udp connection to the other client (sending)

			pthread_join(cId3, NULL);
			pthread_join(cId4, NULL);
			pthread_join(cId2, NULL);
		}
	}
	else                                   //wrong input
		printf("Username or Password is incorrect\n");
	
	close(sockPT);
	free(info1);
	free(info2);
	free(data);
	free(message);

	return 0;
}
