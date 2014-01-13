#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

struct signupData
{
	char fullName[30];
	char username[30];
	char password[30];
	char email[30];
	char securityQuestion[30];
	char securityAnswer[30];
};

struct signinData
{
	char username[30];
	char password[30];
};


void signupClient(int sock);

int signinClient(int sock);

void createSocket(int* sock);

void acceptNewClient(int sock, int* next_sock);

size_t recieveFromClient(int sock, void *message, size_t num);

size_t sendToClient(int sock, const void *message, size_t num);

int scanDatabase(struct signinData *resData, FILE *DB);

void sendOnlineClients(int sock);

void scanOnlineClients(char* client);

int checkExistance(char *username, char* select);

