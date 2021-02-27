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
// chmod 700 server.c | gcc -Wall -D_REENTRANT -pthread -o sv server.c

int ServerSocket;
struct sockaddr_in ServerAddress;
int ClientSocket[50];
char smd[30];

typedef struct node{
    char NumeFisier[25];
    struct node *urm;
}node;

typedef struct ClientData{
    char Nume[25];
    struct node *Fisier;
}ClientData;

struct ClientData *DataClient;

node *CreeateNode(char *fisier)
{
    struct node *Nod=malloc(sizeof(struct node));
    strcpy(Nod->NumeFisier,fisier);
    Nod->urm=NULL;
    return Nod;
}

node * AddFile(node *Nod,char *filename)
{
    struct node *Nodint=CreeateNode(filename);
    struct node *p=Nod;
    while(p->urm !=NULL)
        p=p->urm;
    p->urm=Nodint;
    return Nod;
}

void parcurgereFisiere(node *Nod)
{
    printf("{ ");
    while(Nod->urm!=NULL)
    {
        printf("%s  -> ",Nod->NumeFisier);
        Nod=Nod->urm;
    }
    printf("%s }.",Nod->NumeFisier);
    printf("\n");
}

ClientData *CreereClient(char *name)
{
    struct ClientData *Client = malloc(sizeof(struct ClientData));
    strcpy(Client->Nume,name);
    Client->Fisier=NULL;
    return Client;
}

ClientData *AdaugareFisier(ClientData *Client,char *numeFisier)
{
    if(Client->Fisier==NULL)
    {
        Client->Fisier=CreeateNode(numeFisier);
    }
    else
    Client->Fisier=AddFile(Client->Fisier,numeFisier);
    return Client;
}

void PrintareClient(ClientData *Client)
{
    printf("\n%s\n",Client->Nume);
    parcurgereFisiere(Client->Fisier);
    printf("\n");
}

void checkdir()
{
	struct stat info;
	int fd;
	if(lstat("server",&info)<0)
	{
		printf("director inexistent, acesta se creaza\n");
		mkdir("server",0777);
		system("touch server/clientslist");
	}
	else
		if(S_ISDIR(info.st_mode))
		{
			printf("director existent\n");	
			if(( fd = open("server/clientslist",O_CREAT | O_EXCL | O_WRONLY, S_IRWXU))<0)
			{
				printf("Fisier existent\n");
				close(fd);
			}
			else
			{
				system("touch server/clientslist");
			}

			
		}

}


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
	pthread_t thread[10];
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
	DataClient=malloc(sizeof(struct ClientData)*40);
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