#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>


struct signupData // sign up structure
{
	char fullName[30];
	char username[30];
	char password[30];
	char email[30];
	char securityQuestion[30];
	char securityAnswer[30];
};

struct signinData // sign in structure
{
	char username[30];
	char password[30];
};


void signupInformation(struct signupData** reg);

void signinInformation(struct signinData** reg);

void createSocket(int* sockPT);

void signupToServer(int sockPT, struct signupData* reg);

void signinToServer(int sockPT, struct signinData* reg);

size_t sendToServer(int sockPT, const void *message, size_t num);

size_t recieveFromServer(int sockPT, void *recieve_message, size_t num);

void recieveOnlineClients(int sockPT, char* buffer);
