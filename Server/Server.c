#include "Server.h"


void signupClient(int sock)
{
	printf("client is signing up...\n");
	struct signupData *resData= malloc(sizeof(struct signupData));
	FILE* DB = fopen("Database.txt", "a");
	int result = 0;	
	
	recieveFromClient(sock, resData, sizeof(struct signupData));
	result = checkExistance(resData->username, "database");
	if(result == 1){
		fprintf(DB, "Username: %s ", resData->username);
		fprintf(DB, "Password: %s ", resData->password);		
		fprintf(DB, "Fullname: %s ", resData->fullName);
		fprintf(DB, "Email: %s ", resData->email);
		fprintf(DB, "Security Question: %s ", resData->securityQuestion);
		fprintf(DB, "Security Answer: %s \n", resData->securityAnswer);
	}
	free(resData);
	fclose(DB);
}

int signinClient(int sock)
{
	int result1 = 0, result2 = 0;

	printf("client is signing in...\n");
	struct signinData *resData= malloc(sizeof(struct signinData));
	struct sockaddr_in sockAddress;
	int addrlen = sizeof(struct sockaddr); 
	
	recieveFromClient(sock, resData, sizeof(struct signinData));
	
		FILE* DB = fopen("Database.txt", "r");
		result1 = scanDatabase(resData, DB);		
		fclose(DB);
		result2 = checkExistance(resData->username, "online_clients");

		if (result1 == 1 && result2 == 1){
			FILE *online_clients = fopen("online_clients.txt", "a");
			getpeername(sock, (struct sockaddr*)&sockAddress, &addrlen);
			fprintf(online_clients, "%d %s %s\n", sock, resData->username, inet_ntoa(sockAddress.sin_addr));
			fclose(online_clients);
			printf("sign in was successful\n");
			return 1;
		}
	

	return 0;
}

void createSocket(int* sock)
{
	struct sockaddr_in serverAddress;
	struct hostent *serverad;

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if(*sock<0){
		printf("Error: Socket can't be created\n");
		error(0);
	}

	bzero((char*)&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(8887);

	if(bind(*sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress))<0){
		printf("Error: The server can't bind...\n");
		exit(0);
	}
	else
		printf("the port and address have been binded\n");

}

void acceptNewClient(int sock, int* next_sock)
{
	int sock_size;
	struct sockaddr_in clientAddress;

	sock_size = sizeof(clientAddress);
	*next_sock = accept(sock, (struct sockaddr*)&clientAddress, &sock_size);
	if(*next_sock<0)
		printf("Error: Server can't accept a new client\n");
	else
		printf("The new client has been accepted\n");
}

size_t recieveFromClient(int sock, void *message, size_t num)
{	
	size_t num_read;
	size_t num_remained = num;
	char *tmp_message = message;

	while(num_remained > 0){
		num_read = read(sock, tmp_message, num_remained);	
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

size_t sendToClient(int sock, const void *message, size_t num)
{
	size_t num_written;
	size_t num_remained = num;
	const char *tmp_message = message;

	while(num_remained > 0){
		num_written = write(sock, tmp_message, num_remained);
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

int scanDatabase(struct signinData *resData, FILE *DB)
{
	char username[30] = {0};
	char password[30] = {0};
	int result = 0;
	char Line[256] = {0};


		while(fgets(Line, 255, DB) != NULL){
			sscanf(Line, "%*s %s %*s %s", username, password);
			if(strcmp(username, resData->username)==0 && strcmp(password, resData->password)==0){
				result = 1;
				break;
			}
		}
	
	return result;
}

void sendOnlineClients(int sock)
{
	char buffer[1024];
	char str[30];
	char Line[256];
	bzero(buffer, 1023);
	bzero(Line, 255);
	bzero(str, 30);

	FILE *online_clients = fopen("online_clients.txt", "r");
	if(online_clients == NULL)
		printf("cannot open the online_clients file\n");

	while(fgets(Line, 256, online_clients) != NULL){
		sscanf(Line, "%*d %s", str);
		strcat(buffer, str);
		strcat(buffer, "\n");
	}
	printf("buffer is: %s\n", buffer);
	printf("server is sending the list of online clients to the client\n");
	if(send(sock, buffer, 1023, 0) < 0)
		printf("cannot send list of online clients to the client\n");

	fclose(online_clients);
}

void scanOnlineClients(char *client)
{
	char Line[256] = {0};
	char tmp[30] = {0};
	FILE *online_clients = fopen("online_clients.txt", "r");		
	
	while(fgets(Line, 255, online_clients) != NULL){
		sscanf(Line, "%*d %s", tmp);
		if(strcmp(tmp, client) == 0)
			sscanf(Line, "%*d %*s %s", client);
	}
	fclose(online_clients);
}

int checkExistance(char *username, char* select)
{
	char Line[256] = {0};
	char str[30] = {0};
	
	if(strcmp(select, "online_clients") == 0){
		FILE *online_clients = fopen("online_clients.txt", "r");
		if(online_clients == NULL)
			printf("cannot open the online_clients file\n");

		while(fgets(Line, 256, online_clients) != NULL){
			sscanf(Line, "%*d %s", str);
			if(strcmp(username, str) == 0)
				return 0;
		}
		fclose(online_clients);	
	}	

	else if(strcmp(select, "database") == 0){
		FILE* DB = fopen("Database.txt", "r");
		if(DB == NULL)
			printf("cannot open the online_clients file\n");

		while(fgets(Line, 255, DB) != NULL){
			sscanf(Line, "%*s %s", str);
			if(strcmp(username, str) == 0)
				return 0;
		}
		fclose(DB);
	}
	return 1;
}

void removeClient(int new_sock)
{
	FILE* DB = fopen("online_clients.txt", "r");
	FILE* DB_copy = fopen("online_clients_copy.txt", "w");
	char Line[256] = {0};
	int sock;
	
	while(fgets(Line, 255, DB) != NULL){
		sscanf(Line, "%d", &sock);
		if(sock != new_sock)
			fprintf(DB_copy, "%s", Line);
	}
	fclose(DB);
	fclose(DB_copy);
	remove("online_clients.txt");
	rename("online_clients_copy.txt", "online_clients.txt");
}
