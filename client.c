#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>

#define LENGTH 1024

int NetworkSocket;
struct sockaddr_in ServerAddress, ServerAddressp;
int ConnectionStatus;
char Name[25];
volatile sig_atomic_t disconnectClient = 0;
int fd;

void printMyFiles(char *directorName) {
	DIR *director;
	if((director = opendir(directorName)) == NULL) {
		printf("Eroare la opendir, nu s-a putut deschide directorul\n");
		exit(500);
	}

	struct dirent *intrareDirector;

	while((intrareDirector = readdir(director)) != NULL) {
		char numeIntrare[30];
		strcpy(numeIntrare, intrareDirector -> d_name);

		if(strcmp(numeIntrare, "..") != 0 && strcmp(numeIntrare, ".") != 0) {
			char fullPath[50];
			snprintf(fullPath, sizeof(fullPath), "%s/%s", directorName, numeIntrare);
			struct stat buf;

			if(lstat(fullPath, &buf) < 0) {
				printf("Eroare la lstat (fullPath)\n");
				exit(501);
			}

			if(S_ISREG(buf.st_mode)) {
				printf("%s |", numeIntrare);
			}
		}
	}

	printf("\n");

}

void Autentificare() {
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

		if(message[0] == 'a' && message[1] == 'd' && message[2] == 'd') {
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

		if(strcmp(message, "seeFiles") == 0) {
			printf("Showing all avaiable files...\n");
		}

		if(strcmp(message, "seeMyFiles") == 0) {
			printf("Showing your files...\n");
			char myDir[33];
			snprintf(myDir, sizeof(myDir), "server/%s", Name);
			printMyFiles(myDir);
		}

		if(strcmp(message, "download") == 0) {
			char f[30];
			send(NetworkSocket, message, strlen(message), 0);
			bzero(message, LENGTH);

			printf("file=");
			fgets(message, LENGTH, stdin);

			fileFlag=1;

			if(message[strlen(message) - 1] == '\n')
				message[strlen(message) - 1] = '\0';

			strcpy(f, message);
			send(NetworkSocket, message, strlen(message), 0);

			char *prs = malloc(sizeof(char) * LENGTH);
			snprintf(prs, LENGTH-1, "server/%s/%s", Name, f);

			if((fd = open(prs,O_CREAT | O_EXCL | O_WRONLY, S_IRWXU)) < 0) {
				printf("Fisier existent\n");
				close(fd);
			}

			free(prs);

		}

		if(!fileFlag)
			send(NetworkSocket, message, strlen(message), 0);

		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);

	}

	stopClient(2);

}


/*
	connection
	file.name
	port
	size
	order
	number

	exemple

	connection
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

void receiveMessageHandler() {

	char message[LENGTH] = {};

  	while (1) {

		int receive = recv(NetworkSocket, message, LENGTH, 0);

		if (receive > 0) {
			printf("%s\n", message);

			if(strcmp(message, "peer") == 0) {
				printf("Primesc date!\n");
				char prs[LENGTH] = {};

				int portnumber;
				char *buff = malloc(sizeof(char) * 5000);

				if(recv(NetworkSocket, prs, LENGTH, 0) < 0) {
					printf("eroare la trimitere spre network\n");
					exit(51);
				}

				printf("%s\n", prs);
				portnumber = atoi(prs);
				memset(prs, 0x00, LENGTH);
				int opt = 1;
				int PeerConection = socket(AF_INET, SOCK_STREAM, 0);
				struct sockaddr_in PeerAddress;

				if(setsockopt(PeerConection, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
					printf("setsockopt");
					exit(45);
				}

				PeerAddress.sin_family = AF_INET;
				PeerAddress.sin_port = htons(portnumber);
				PeerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

				if(bind(PeerConection, (struct sockaddr *) &PeerAddress, sizeof(PeerAddress)) < 0) {
					printf("Eroare la socket binding\n");
					exit(282);
				}

				if(listen(PeerConection, 3) < 0) {
					printf("Eroare la socket listening\n");
					exit(283);
				}

				int newsocket;

				if((newsocket = accept(PeerConection, NULL, NULL)) == -1) {
					printf("Eroare la acceptare!\n");
					exit(43);
				}

				int n;

				while((n = recv(newsocket, buff, sizeof(buff), 0)) > 0) {
					printf("%s -buff nr = %d\n", buff, n);

					if(write(fd, buff, n) < 0) {
						printf("Eroare la scriere");
						exit(34);
					}

					memset(buff, 0x00, LENGTH);
				}

				printf("buffer gol\n");

			}

			if(strcmp(message, "connection") == 0) {
				char Datatypefiles[50];
				char denumire[30];
				int port;
				int size;
				int order;
				int number;
				recv(NetworkSocket, &Datatypefiles, sizeof(Datatypefiles), 0);
				printf("Am primit un request\n%s\n", Datatypefiles);
				char *p = strtok(Datatypefiles, "\n");

				strcpy(denumire,p);
				p = strtok(NULL, "\n");
				port = atoi(p);
				p = strtok(NULL, "\n");
				size = atoi(p);
				p = strtok(NULL, "\n");
				order = atoi(p);
				p = strtok(NULL, "\n");
				number = atoi(p);
				int dim = size / number + 1;

				int fd2;

				char *path = malloc(sizeof(char) * 200);
				struct stat info;
				snprintf(path, 199, "server/%s/%s", Name, denumire);
				printf("%s\n", path);

				if((fd2 = open(path,O_RDONLY |S_IRUSR)) < 0) {
					printf("Eroare la deschidere fisier\n");
					exit(4);
				}

				printf("Inainte de fstat\n");

				if(fstat(fd2, &info) < 0) {
					printf("Eroare la fc stat\n");
					exit(69);
				}

				lseek(fd2, (info.st_size / number * order), SEEK_CUR);

				char *buff = malloc(sizeof(char) * (dim + 1));

				int n;
				printf("Inainte de read\n");

				if((n = read(fd2, buff, dim)) < 0) {
					printf("Eroare la citirea din fisier\n");
					exit(420);
				}
				else {
					int PeerSocket;
					PeerSocket = socket(AF_INET, SOCK_STREAM, 0);

					ServerAddressp.sin_family = AF_INET;
					ServerAddressp.sin_port = htons(port);
					ServerAddressp.sin_addr.s_addr = inet_addr("127.0.0.1");
					printf("Inainte de Connection\n");
					ConnectionStatus = connect(PeerSocket, (struct sockaddr *) &ServerAddressp, sizeof(ServerAddressp));

					if(ConnectionStatus == -1) {
						printf("Connection failed!\n");
						exit(1);
					}

					printf("%s\n\n", buff);
					printf("Inainte de send\n");
					send(PeerSocket, buff, strlen(buff), 0);

					close(fd2);
					free(buff);
					close(PeerSocket);
				}
			}

			flushStdout();

		}

		else
			if (receive == 0) {
				close(fd);
				break;
			}

			else {
				// -1
			}

		memset(message, 0, sizeof(message));

  	}
}

void createAndHoldConnection() {

	char ServerResponse[11];
	NetworkSocket = socket(AF_INET, SOCK_STREAM, 0);

	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(2424);
	ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	ConnectionStatus = connect(NetworkSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress));

	if(ConnectionStatus == -1) {
		printf("Connection failed!\n");
		exit(1);
	}

	send(NetworkSocket, Name, sizeof(Name), 0);
	recv(NetworkSocket, &ServerResponse, sizeof(ServerResponse), 0);
	printf("%s \n", ServerResponse);

	pthread_t send_msg_thread;

  	if(pthread_create(&send_msg_thread, NULL, (void *) sendMessageHandler, NULL) != 0) {
		printf("ERROR: pthread\n");
   		exit(300);
	}

	pthread_t recv_msg_thread;

  	if(pthread_create(&recv_msg_thread, NULL, (void *) receiveMessageHandler, NULL) != 0) {
		printf("ERROR: pthread\n");
		exit(301);
	}

	while(1) {
		if(disconnectClient) {
			printf("You have disconnected!\n");
			break;
		}
	}

	close(NetworkSocket);

}

int main() {

	Autentificare();
	createAndHoldConnection();

	return 0;

}
