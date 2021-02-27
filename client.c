#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <unistd.h>
int NetworkSocket;
struct sockaddr_in ServerAddress;
int ConnectionStatus;
char Name[25];
void Autentificare()
{
	printf("Welcome young padawan!\n\n");
	printf("We are trying to connect to the server.\n\n");
	printf("But wait, who are u???\n");
	scanf("%s",Name);
}

void createConnection()
{
	char ServerResponse[11];
	NetworkSocket=socket(AF_INET,SOCK_STREAM,0);

	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(2424);
	ServerAddress.sin_addr.s_addr=INADDR_ANY;

	ConnectionStatus = connect(NetworkSocket, (struct sockaddr *) &ServerAddress,sizeof(ServerAddress));
	if(ConnectionStatus == -1)
	{
		printf("Connection failed!\n");
		exit(1);
	}
	
	recv(NetworkSocket,&ServerResponse,sizeof(ServerResponse),0);
	printf("%s \n",ServerResponse);
	send(NetworkSocket,Name,sizeof(Name),0);

}

void holdconnection()
{
	char command[30];
	int sw=1;
	
	printf("type help for help\n");

	while(sw)
	{	
		scanf("%s",command);
		send(NetworkSocket,command,strlen(command),0);
		
		if(strcmp(command,"stop")==0)
			sw=0;
		memset(&command,0x00,sizeof(command));


	}

}


int main()
{
	Autentificare();
	createConnection();
	holdconnection();
	return 0;
}