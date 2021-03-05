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
#include "disc.h"
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
	struct stat info;
	while(sw)
	{
		memset(&command,0x00,sizeof(command));

		recv(ClientSocket[i],&command,sizeof(command),0);

		

		if(strcmp(command,"stop")==0)
			sw=0;

		if(strcmp(command,"add")==0)
		{		printf("add\n");
			char file[30];
			memset(&file,0x00,sizeof(file));
			recv(ClientSocket[i],&file,sizeof(file),0);
			printf("file= %s\n",file);
			if(lstat(file,&info)<0)
			{
			printf("Eroare la functia stat %s\n",file);
			exit(99);
			}
			else
				if(S_ISREG(info.st_mode))
				{
					int fd1,fd2;
					int n;
					char *buff=malloc(sizeof(char)*1024);
					if((fd1=open(file,O_RDONLY))<0)
					{
						printf("Eroare la deschidere fisier\n");
						exit(75);
					}
					else
					{
						
						snprintf(buff,50,"server/%s/%s",Name,file);

						if((fd2=open(buff,O_CREAT|O_WRONLY|O_EXCL,S_IRWXU))<0)
						{
							printf("Eroare la creare fisier\n");
							exit(69);
						}

						while((n=read(fd1,buff,1024))>0)
						{
							if(write(fd2,buff,strlen(buff))<0)
							{
								printf("Eroare la scrierea in fisier\n");
								exit(24);
							}
						}
					}
					close(fd1);
					close(fd2);
					free(buff);
				}


			
		}
		printf("%s : %s\n",Name,command);
		PrintareClient(&DataClient[i]);
	}

}

void * threadClient( void *arg )
{
	char Name[25];
	int i = *((int *) arg);
	printf("i=%d\n",i);
	sendMessage("Connected!",i);
	strcpy(Name,receiveMessage(i));
	newUser(Name,DataClient,i);
	DataClient[i]=*CreereClient(Name);
	PrintareClient(&DataClient[i]);
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