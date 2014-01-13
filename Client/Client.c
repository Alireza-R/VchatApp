#include "Client.h"

// this function ask for user information for sign up process to the server
void signupInformation(struct signupData** reg) 
{
	printf("Please enter your personal information...\n");
	printf("Enter your fullname: ");
	scanf("%s", (*reg)->fullName);

	printf("Enter your prefered Username: ");
	scanf("%s", (*reg)->username);

	strcpy((*reg)->password, getpass("Enter your password: "));	

	printf("Enter your email address: ");
	scanf("%s", (*reg)->email);

	printf("Choose a security question: ");
	scanf("%s", (*reg)->securityQuestion);

	printf("Enter your securityAnswer: ");
	scanf("%s", (*reg)->securityAnswer);
}
// in this function the user should submit the authentication information for sign in process
void signinInformation(struct signinData** reg)
{
	printf("Enter your prefered Username: ");
	scanf("%s", (*reg)->username);

	strcpy((*reg)->password, getpass("Enter your password: "));	
}

// this function create the socket and make connection to the server (TCP connection)
void createSocket(int* sockPT)
{
	struct sockaddr_in serverAddress;
	struct hostent *serverad;

	*sockPT = socket(AF_INET, SOCK_STREAM, 0);
	if(*sockPT<0){
		printf("Error: Socket can't be created\n");
		error(0);
	}
	serverad = gethostbyname("alirezaserver.servehttp.com");
	if(serverad == NULL){
		printf("Error: no address for server is available.\n");
		exit(0);
	}
	else
		printf("Server is running\n");

	bzero((char*)&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	bcopy((char*)serverad->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverad->h_length);
	serverAddress.sin_port = htons(8887);

	if(connect(*sockPT, (struct sockaddr*)&serverAddress, sizeof(serverAddress))<0){
		printf("Error: The client can't connect...\n");
		exit(0);
	}
	else
		printf("Connection established\n");
}

//sending registration information to the server
void signupToServer(int sockPT, struct signupData* reg)
{
	sendToServer(sockPT, reg, sizeof(struct signupData));
}

//sending sign in information to the server
void signinToServer(int sockPT, struct signinData* reg)
{
	sendToServer(sockPT, reg, sizeof(struct signinData));
}

//send a message to the server with different size (TCP connection)
size_t sendToServer(int sockPT, const void* message, size_t num)
{
	size_t num_written;
	size_t num_remained = num;
	const char *tmp_message = message;

	while(num_remained > 0){
		num_written = write(sockPT, tmp_message, strlen(tmp_message)+1);	
		if(num_written <= 0){		
			if(num_written < 0 && errno == EINTR)		
				num_written = 0;
			else
				return (-1);
		}

		num_remained -= num_written;
		tmp_message += num_written;
	}
	return (num);	
	
	}

//recieve a message to the server with different size (TCP connection)
size_t recieveFromServer(int sockPT, void *message, size_t num)
{
	size_t num_read;
	size_t num_remained = num;
	char *tmp_message = message;

	while(num_remained > 0){
		num_read = read(sockPT, tmp_message, num_remained);
		if(num_read < 0){		
			if(errno == EINTR)		
				num_read = 0;
			else
				return (-1);
		}
		else if(num_read == 0)
			break;
		num_remained -= num_read;
		tmp_message += num_read;
	}
	return (num - num_remained);
}

// Recieve the list of online clients from server. This list will be used to find the name of available users
void recieveOnlineClients(int sockPT, char* buffer)
{

	FILE *online_clients = fopen("online_clients.txt", "w");
	
	printf("client is recieving the list of online clients\n");
	if(recv(sockPT, buffer, 1023, 0)<0)
		printf("cannot recieve the number of clients\n");
	
	fprintf(online_clients, "%s", buffer);	

	printf("*****online clients******\n");
	printf("%s\n", buffer);
	printf("*************************\n");

	fclose(online_clients);

}

// create a database to store the name of online users for future updates
int scanDatabase(char *online_clients, char* client)
{
	int i = 0, j = 0;
	char *tmp = malloc(30);
	while(online_clients[i] != '\0'){
		bzero(tmp, 30);
		j = 0;
		while(online_clients[i] != '\n')
			tmp[j++] = online_clients[i++];
		tmp[j] = '\0';
		if(strcmp(tmp, client) == 0)
			return 1;
		i++;
	}
	free(tmp);
	return 0;
}

//send data over udp connection to the other client
void sendToClient(int sockfd, char* message, char* IPaddress, int size)
{
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(IPaddress);
	servaddr.sin_port = htons(31000);

	sendto(sockfd, message, size, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	close(sockfd);
}

//recieve data over udp connection to the other client
void recieveFromClient(int sockfd, char* message, int size)
{
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(31000);

	bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	recvfrom(sockfd, message, size, 0, NULL, NULL); 
	close(sockfd);
}
