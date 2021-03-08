#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include<pthread.h>
int NetworkSocket;
struct sockaddr_in ServerAddress,ServerAddressp;
int ConnectionStatus;
char Name[25];
volatile sig_atomic_t disconnectClient = 0;
#define LENGTH 1024

void printMyFiles(char *directorName)
{	
	DIR *director;
	if((director = opendir(directorName)) == NULL)
	{
		printf("Eroare la opendir, nu s-a putut deschide directorul\n");
		exit(500);
	}

	struct dirent *intrareDirector;
	while((intrareDirector = readdir(director)) != NULL)
	{
		char numeIntrare[30];
		strcpy(numeIntrare, intrareDirector->d_name);

		if(strcmp(numeIntrare, "..")!=0 && strcmp(numeIntrare, ".")!=0)
		{
			char fullPath[50];
			snprintf(fullPath, sizeof(fullPath), "%s/%s", directorName, numeIntrare);

			struct stat buf;
			if(lstat(fullPath, &buf) < 0)
			{
				printf("Eroare la lstat (fullPath)\n");
				exit(501);
			}

			if(S_ISREG(buf.st_mode))
			{
				printf("%s |", numeIntrare);
			}
		}
	}
		printf("\n"); 
}

void Autentificare()
{
	printf("Welcome young padawan!\n\n");
	printf("We are trying to connect to the server.\n\n");
	printf("But wait, who are u???\n");
	printf("Enter name: ");
	fgets(Name, 32, stdin); // replace in all files with scanf after fin. code(onyl 1 word commands allowed, else #unknown#)
  	if(Name[strlen(Name) - 1] == '\n')
  		Name[strlen(Name) - 1] = '\0';
}

void rmNewLine (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void stopClient(int sig) {
    disconnectClient = 1;
}

void flushStdout() {
  printf("%s", "> ");
  fflush(stdout);
}

void sendMessageHandler() {
  char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};
	int fileFlag = 0;
	printf("Type 'help' for help\n");

  while(1) {
  	fileFlag = 0;
  	flushStdout();
    fgets(message, LENGTH, stdin);
   	if(message[strlen(message) - 1] == '\n')
  		message[strlen(message) - 1] = '\0';

    if (strcmp(message, "stop") == 0) 
			break;

	if(strcmp(message, "help") == 0)
		printf("Valid commands: stop, add file.extension, seeFiles, seeMyFiles\n");

	if(message[0] == 'a' && message[1] == 'd' && message[2] == 'd')
	{
		char file[30];
		int fileIndex = 0;

		for(int k = 4; k < strlen(message); k++)
			file[fileIndex++] = message[k];

		char clientDirFile[100];
		snprintf(clientDirFile, 100, "server/%s/%s", Name, file);

		int e1, e2;
		// verify if the file already exists OR if it exists at all
			if((e1 = access(file, F_OK)) != 0)
				printf("Error: File %s doesn't exist or its path is invalid.\n", file);
			else
				if((e2 = access(clientDirFile, F_OK)) == 0)
					printf("Error: File %s already exists in your dir = server/%s.\n", file, Name);
							
		if(e1 == 1 || e2 == 0)
			fileFlag = 1;
	}

	if(strcmp(message, "seeFiles") == 0)
	{
		printf("Showing all avaiable files...\n");
	}

	if(strcmp(message, "seeMyFiles") == 0)
	{
		printf("Showing your files...\n");
		char myDir[33];
		snprintf(myDir, sizeof(myDir), "server/%s", Name);
		printMyFiles(myDir);
	}
		if(!fileFlag)
			send(NetworkSocket, message, strlen(message), 0);

		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);
  }

  stopClient(2);
}

void receiveMessageHandler() {
	char message[LENGTH] = {};
  while (1) {
		int receive = recv(NetworkSocket, message, LENGTH, 0);
    if (receive > 0) {
      printf("%s\n", message);
      flushStdout();
    } else if (receive == 0) {
			break;
    } else {
			// -1
		}
		memset(message, 0, sizeof(message));
  }
}

void createAndHoldConnection()
{
	char ServerResponse[11];
	NetworkSocket=socket(AF_INET,SOCK_STREAM,0);

	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(2424);
	ServerAddress.sin_addr.s_addr=inet_addr("127.0.0.1");

	ConnectionStatus = connect(NetworkSocket, (struct sockaddr *) &ServerAddress,sizeof(ServerAddress));
	if(ConnectionStatus == -1)
	{
		printf("Connection failed!\n");
		exit(1);
	}
	send(NetworkSocket,Name,sizeof(Name),0);
	recv(NetworkSocket,&ServerResponse,sizeof(ServerResponse),0);
	printf("%s \n",ServerResponse);

	pthread_t send_msg_thread;
  	if(pthread_create(&send_msg_thread, NULL, (void *) sendMessageHandler, NULL) != 0){
		printf("ERROR: pthread\n");
   		exit(300);
	}

	pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) receiveMessageHandler, NULL) != 0){
		printf("ERROR: pthread\n");
		exit(301);
	}

	while(1)
	{
		if(disconnectClient)
		{
			printf("You have disconnected!\n");
			break;
		}
	}
		close(NetworkSocket);
}

int main()
{
	
	Autentificare();
	createAndHoldConnection();
	return 0;
}
