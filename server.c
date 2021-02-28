#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

#include "structura.h"
#define MAX_SIZE 10

// chmod 700 server.c | gcc -Wall -D_REENTRANT -pthread -o sv server.c
int ServerSocket;
struct sockaddr_in ServerAddress;
int ClientSocket[MAX_SIZE];
char smd[30];
struct ClientData *DataClient;




void sendMessage(char *ServerMessage,int i)
{	
	
	if(sendto(ClientSocket[i],ServerMessage,strlen(ServerMessage),0,(struct sockaddr *)&ServerAddress,sizeof(ServerAddress))<0)
	{
		printf("Eroare la trimitere");
		exit(4);
	}
	
}

char * receiveMessage(int i)
{
	
	recv(ClientSocket[i],&smd,sizeof(smd),0);
	printf("%s has connected\n",smd);
	return smd;
	
}

void commands(int i,char *Name)
{
	int sw=1;
	char command[30];

	while(sw)
	{
		memset(&command,0x00,sizeof(command));

		recv(ClientSocket[i],&command,sizeof(command),0);

		printf("%s : %s\n",Name,command);

		if(strcmp(command,"stop")==0)
			sw=0;

	}

}

void * threadClient( void *arg )
{
	char Name[25];
	int i = *((int *) arg);
	sendMessage("Connected!",i);
	strcpy(Name,receiveMessage(i));
	//DataClient[i]=*CreereClient(Name);
	commands(i,Name);
	printf("%s has disconnected!\n",Name);
	pthread_exit(NULL);
}

void WaitForOtherClients()
{		
	pthread_t thread[MAX_SIZE];
	int i=0;
	while(1)
	{
		if( (ClientSocket[i] = accept(ServerSocket,NULL,NULL)) == -1)
			printf("Accepted failed at %d",i);
		else
			{
					if(pthread_create(&thread[i],NULL,threadClient,&i) !=0 )
					printf("Failed to create thread\n");
			
				
			}
		printf("%d",i);
		
		
	}
}

void createConnection()
{
	DataClient=malloc(sizeof(struct ClientData)*MAX_SIZE);
	ServerSocket = socket(AF_INET,SOCK_STREAM,0);
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(2424);
	ServerAddress.sin_addr.s_addr = INADDR_ANY;
	bind(ServerSocket,(struct sockaddr *)&ServerAddress,sizeof(ServerAddress));
	listen(ServerSocket,5);
	WaitForOtherClients();
	
}





int main()
{
	checkdir();
	createConnection();
	return 0;
}