#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include<pthread.h>
int NetworkSocket;
struct sockaddr_in ServerAddress,ServerAddressp;
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


/*
file.name
port
size
order
number

exemple

cat.mp4
2425
1255
3
5

=> dimensiune 1255 , 5 persoane care trimit
1255/5 = 251
primul   -> 0   -> 251
al 2-lea -> 251 -> 502
al 3-lea -> 502 -> 753
al 4-lea -> 753 -> 1004
al 5-lea -> 1004-> 1255

Ordinul 3 => eu o sa trimit prin portul 2425 din fisierul cat.mp4 de la 502 pana la 753.

*/

void * sendFiles(void *arg)
{
	char Datatypefiles[50];
	char denumire[30];
	int port;
	int size;
	int order;
	int number;
	recv(NetworkSocket,&Datatypefiles,sizeof(Datatypefiles),0);

	if(strcmp(Datatypefiles,"stop"))
		pthread_exit(NULL);
	
	char *p = strtok(Datatypefiles,"\n");
	strcpy(denumire,p);
	p=strtok(NULL,"\n");
	port=atoi(p);
	p=strtok(NULL,"\n");
	size=atoi(p);
	p=strtok(NULL,"\n");
	order=atoi(p);
	p=strtok(NULL,"\n");
	number=atoi(p);
	int dim=size/number+1;


	int fd;

	char *path=malloc(sizeof(char)*200);
	struct stat info;
	snprintf(path,199,"server/%s/%s",Name,denumire);

	if((fd=open(path,O_RDONLY |S_IRUSR))<0)
	{
		printf("Eroare la deschidere fisier\n");
		exit(4);
	}

	if(fstat(fd,&info)<0)
	{
		printf("Eroare la fc stat\n");
		exit(69);
	}

	lseek(fd,(info.st_size/number*order),SEEK_CUR);

	char *buff=malloc(sizeof(char)*(dim+1));

	int n;
	if((n=read(fd,buff,dim))<0)
	{
		printf("Eroare la citirea din fisier\n");
		exit(420);
	}

	else
	{

	int PeerSocket;

	PeerSocket=socket(AF_INET,SOCK_STREAM,0);

	ServerAddressp.sin_family = AF_INET;
	ServerAddressp.sin_port = htons(port);
	ServerAddressp.sin_addr.s_addr=INADDR_ANY;

	ConnectionStatus = connect(PeerSocket, (struct sockaddr *) &ServerAddressp,sizeof(ServerAddressp));
	if(ConnectionStatus == -1)
	{
		printf("Connection failed!\n");
		exit(1);
	}

	send(PeerSocket,buff,strlen(buff),0);

	close(fd);
	free(buff);
	pthread_exit(NULL);
}
}

void holdconnection()
{
	char command[30];
	int sw=1;
	pthread_t thread;
	if(pthread_create(&thread,NULL,sendFiles,NULL)!=0)
	{
		printf("Failed to create thread\n");
	}
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

	pthread_join(thread,NULL);

}


int main()
{
	
	Autentificare();
	createConnection();
	holdconnection();
	return 0;
}
