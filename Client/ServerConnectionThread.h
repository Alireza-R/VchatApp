#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

struct thread{
int sock;
char IPaddr[30];
uint32_t Port;
char client[30];
};

void* ServerConnectionThread(void* sock);
