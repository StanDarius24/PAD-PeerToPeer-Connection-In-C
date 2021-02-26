#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>
int ServerSocket;
struct sockaddr_in ServerAddress;
int ClientSocket[50];

void sendMessage(char *ServerMessage,int i)
{	
	send(ClientSocket[i], ServerMessage,sizeof(ServerMessage),0);
	
}

void receiveMessage(int i)
{
	char Name[25];
	recv(ClientSocket[i],&Name,sizeof(Name),0);
	printf("%s has connected\n",Name);
	
}

void * message( void *arg)
{
	int i = *((int *) arg);
	sendMessage("Connected!",i);
	receiveMessage(i);
	pthread_exit(NULL);
}

void WaitForOtherClients()
{	
	pthread_t thread[10];
	int i=0;
	while(1)
	{
		if( (ClientSocket[i] = accept(ServerSocket,NULL,NULL)) == -1)
			printf("Accepted failed at %d",i);
		else
			{
					if(pthread_create(&thread[i],NULL,message,&i) !=0 )
					printf("Failed to create thread\n");
				
			}
		printf("%d",i);
		
		
	}
}


void createConnection()
{
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

createConnection();
	

	return 0;
}