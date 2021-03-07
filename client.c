#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/stat.h>
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
	printf("Enter name: ");
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
		printf("?> ");
		scanf("%s",command);
		send(NetworkSocket,command,strlen(command),0);

		if(strcmp(command,"help")==0)
				printf("Valid commands: stop, add, seeFiles\n"); 

		if(strcmp(command,"stop")==0)
				sw=0; 

		if(strcmp(command,"add")==0)
		{
			char file[30];

			printf("Insert file path: ");
			scanf("%s",file);
			send(NetworkSocket,file,strlen(file),0);

			char clientDirFile[100];
			snprintf(clientDirFile, 100, "server/%s/%s", Name, file);

			// verify if the file already exists OR if it exists at all, server crashes if it already exists and a client tries to add it again
				if(access(file, F_OK) != 0)
					printf("Error: File %s doesn't exist or its path is invalid.\n", file);
				else
					if(access(clientDirFile, F_OK) == 0)
						printf("Error: File %s already exists in server/%s.\n", file, Name);
		}

		if(strcmp(command,"seeFiles")==0) 
		{
			printf("Showing available files...\n");
			char msj[1024];
			int msjLength = recv(NetworkSocket, &msj, sizeof(msj), 0);
			msj[msjLength] = '\0';
			printf("%s\n", msj);
		}

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
