#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#include "structura.h"

#define MAX_SIZE 10
#define MAX_CLIENTS 20

// chmod 700 server.c | gcc -Wall -D_REENTRANT -pthread -o sv server.c structura.o disc.o
int ServerSocket;
struct sockaddr_in ServerAddress;
int ClientSocket[MAX_SIZE];
char smd[30];
static _Atomic unsigned int nrClients = 0;
static int uid = 10;
int port = 2424;

struct node *allFiles;

ClientData *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void AdaugareFisier(int iClient, char *numeFisier) {

    if(clients[iClient] -> Fisier == NULL) {
        clients[iClient] -> Fisier = CreeateNode(numeFisier);
    }
    else
    	clients[iClient] -> Fisier = AddFile(clients[iClient] -> Fisier, numeFisier);
}

void PrintareClient(int iClient) {
	printf("\n%s: ", clients[iClient] -> Nume);

	if(clients[iClient] -> Fisier != NULL)
		parcurgereFisiere(clients[iClient] -> Fisier);
	else
		printf("0 files found\n");
	printf("\n");
}




void flushStdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void rmNewLine (char* arr, int length) {
	int i;

	for (i = 0; i < length; i++) { // trim \n
		if(arr[i] == '\n') {
			arr[i] = '\0';
      		break;
		}
  	}
}

void addToClientsList(ClientData *cl) {
	//pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(!clients[i]) {
			clients[i] = cl;
			break;
		}
	}
	//pthread_mutex_unlock(&clients_mutex);
}

void removeFromClientsList(int uid) {
//	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(clients[i]) {
			if(clients[i] -> uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}

	//pthread_mutex_unlock(&clients_mutex);
}

// sends message back to conn. sender
void sendMessageToClient(char *s, int uid) {
	//pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]) {
			if(clients[i] -> uid == uid){
				if(write(clients[i]->sockfd, s, strlen(s)) < 0)
					printf("Eroare nu s-a putut tr msj 'Connected!' catre client\n");
				break;
			}
		}
	}
//	pthread_mutex_unlock(&clients_mutex);
}

void * threadClient(void *arg) {
	char Name[32];
	nrClients++;
	ClientData *cli = (ClientData *)arg;
	int disconnectClient = 0;
	char cBuffer[1024];

	// receive name
	if(recv(cli -> sockfd, Name, 32 ,0) <= 0) {
		printf("Eoare primire nume in theadClient()\n");
		disconnectClient = 1;
	}
	else {
		strcpy(cli -> Nume, Name);
		snprintf(cBuffer, sizeof(cBuffer), "%s has succesfully connected\n", cli -> Nume);
		printf("%s", cBuffer);
		sendMessageToClient("Connected!", cli -> uid);
	}

	memset(&cBuffer, 0x00, sizeof(cBuffer));

	// handle new users ~ newUser(Name, clients, i): _Name, _clients, i?
	int sw = 1;
	DIR *director, *director2;
	struct dirent *dir, *dir2;
	struct stat info, info2;
	char *name = malloc(sizeof(char) * 30);
	char *path = malloc(sizeof(char) * 50);
	int iClient = 0;

	for(iClient = 0; iClient < nrClients; ++iClient) { // iClient = index client curent
		if(strcmp(clients[iClient] -> Nume, Name) == 0)
			break;
	}

	if(!(director = opendir("server"))) {
		printf("Eroare la deschiderea directorului server\n");
		exit(9);
	}

	while((dir = readdir(director)) > 0 && sw) {
		strcpy(name, dir -> d_name);

		if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0 )
			continue;

		snprintf(path, 49, "server/%s", name);

		if(lstat(path, &info) < 0) {
			printf("Eroare la functia stat\n");
			exit(10);
		}

		if(S_ISDIR(info.st_mode) && (strcmp(Name, name) == 0)) {
			printf("director existent, utilizator vechi\n");
			sw=0;

			if(!(director2 = opendir(path))) {
				printf("Eroare la deschiderea directorului 2\n");
				exit(12);
			}

			char *names = malloc(sizeof(char) * 50);
			char *path2 = malloc(sizeof(char) * 50);

			while((dir2 = readdir(director2)) > 0) {
				strcpy(names, dir2 -> d_name);
				if(strcmp(names, ".") == 0 || strcmp(names, "..") == 0 )
					continue;

				snprintf(path2, 49, "%s/%s", path, names);
				printf("path to files : %s \n", path2);

				if(lstat(path2, &info2) < 0) {
					printf("Eroare la lstat in dir 2\n");
					exit(13);
				}

				if(S_ISREG(info2.st_mode)) {
					printf("Adaugare \n");
					AdaugareFisier(iClient, names);

					if(!FileAlreadyExistsInAllFiles(allFiles, names))
						allFiles = AddToAllFiles(allFiles, names);
				}
			}

			closedir(director2);
			free(names);
			free(path2);

		}
	}

	if(sw==1) {
		printf("Utilizator nou, generam director!\n");

		pid_t child;

		if((child = fork()) < 0) {
			printf("Eroare la proces xD\n");
			exit(112);
		}
		else {
			if(child == 0) {
				char *xD = malloc(sizeof(char) * 112);
				snprintf(xD, 100, "server/%s", Name);
				execlp("mkdir", "mkdir", xD, NULL);
			}
		}
	}

	closedir(director);
	free(name);
	free(path);

	PrintareClient(iClient);

	// receive msj handler | integrated with the former commands(i, Name)
	while(1){

		if(disconnectClient) {
			break;
		}

		memset(cBuffer, 0, sizeof(cBuffer));
		int receive = recv(cli -> sockfd, cBuffer, 1024, 0);

		if (receive > 0) {
			if(strlen(cBuffer) > 0) {
				rmNewLine(cBuffer, strlen(cBuffer));
				printf("%s -> %s\n", cBuffer, cli -> Nume);

				if(cBuffer[0] == 'a' && cBuffer[1] == 'd' && cBuffer[2] == 'd') {
					char file[30];
					memset(file, 0, sizeof(file));
					int fileIndex = 0;

					for(int k = 4; k < strlen(cBuffer); k++)
						file[fileIndex++] = cBuffer[k];
					// terminator?

					printf("%s is trying to host a file '%s'\n", Name, file);

					char clientDirFile[71];
					snprintf(clientDirFile, sizeof(clientDirFile), "server/%s/%s", Name, file);

					AdaugareFisier(iClient, file);
					if(!FileAlreadyExistsInAllFiles(allFiles, file))
						allFiles = AddToAllFiles(allFiles, file);

					if(lstat(file, &info) < 0) {
						printf("Eroare la functia stat %s\n", file);
						exit(99);
					}
					else {
						if(S_ISREG(info.st_mode)) {
							int fd1, fd2;
							int n;
							char *buff = malloc(sizeof(char) * 1025);

							if((fd1 = open(file, O_RDONLY)) < 0) {
								printf("Eroare la deschidere fisier\n");
								exit(75);
							}
							else {
								snprintf(buff,71,"server/%s/%s", Name, file);

								if((fd2 = open(buff, O_CREAT | O_WRONLY | O_EXCL, S_IRWXU)) < 0) {
									printf("Eroare la creare fisier\n");
									exit(69);
								}

								while((n = read(fd1, buff, 1024)) > 0) {
									if(write(fd2, buff, n) < 0) {
										printf("Eroare la scrierea in fisier\n");
										exit(24);
									}
								}
							}

							printf("File succesfully added.\n");
							close(fd1);
							close(fd2);
							free(buff);
						}
					}
				}
				else {
					if(strcmp(cBuffer, "seeFiles") == 0) {
						PrintareAllFiles(allFiles);

				    	char msj[1024], msjAux[20];
				    	struct node *p = allFiles;
				    	int c = 0;

				        while(p != NULL) {

				        	if(c == 0) {
				        		strcpy(msj, p -> NumeFisier);
				        		p = p ->urm;
				        		c = 1;
				        	}
				        	else {
				        		strcpy(msjAux, " | ");
				        		strcat(msjAux, p -> NumeFisier);
				        		strcat(msj, msjAux);
				          		p = p -> urm;
				        	}
				        }

	        			send(cli->sockfd, msj, strlen(msj), 0);
					}
					else {
						if(strcmp(cBuffer, "download") == 0) {
							char *buff = malloc(sizeof(char) * 30);

							recv(cli->sockfd, buff, 30, 0);

							// in buff am numele fisierului ACUM.

							/*

								connection
								file.name
								port
								size
								order
								number

							*/

							printf("Denumirea fisierului e:%s %d\n", buff, nrClients);
							int size;
							int numbs = 0;
							int sockclientswithfiles[20];
							int sw = 0;

							for(int ic = 0; ic < nrClients; ic++) {
								struct node *p = clients[ic] -> Fisier;

								while(p != NULL) {
									if(strcmp(p->NumeFisier,buff) ==0 ) {

										sockclientswithfiles[numbs] = clients[ic] -> sockfd;
										numbs++;

										printf("Nume=%s\n", clients[ic] -> Nume);
										printf("fis=%s \n", p -> NumeFisier);

										if(sw == 0) {
											sw=1;

											struct stat info;
											char *flnm = malloc(sizeof(char) * 41);

											snprintf(flnm, 40, "server/%s/%s", clients[ic] -> Nume, buff);

											if(lstat(flnm, &info) < 0) {
												printf("Eroare la lstat\n");
												exit(21);
											}

											size=info.st_size;
										}
									}

									p = p -> urm;
								}
							}

							int portt = 5125;
							char *detrimis = malloc(sizeof(char) * 1024);
							char *scwc = malloc(sizeof(char) * 22);

							for(int ic = 0; ic < numbs; ic++) {
								if(send(sockclientswithfiles[ic], "connection", strlen("connection"), 0) < 0) {
									printf("Eroare la trimiterea catre peersi\n");
									exit(323);
								}

								snprintf(detrimis, 1023, "%s\n%d\n%d\n%d\n%d\n", buff, portt + ic, size, ic, numbs);

								if(send(sockclientswithfiles[ic], detrimis, strlen(detrimis), 0) < 0) {
									printf("Eroare la trimiterea catre peersi\n");
									exit(323);
								}

								if(send(cli -> sockfd, "peer", strlen("peer"), 0) < 0) {
									printf("Eroare la trimiterea catre client in download\n");
									exit(66);
								}

								snprintf(scwc, 21, "%d", portt + ic);

								if(send(cli -> sockfd, scwc, strlen(scwc), 0) < 0) {
									printf("Eroare la trimiterea catre client in download\n");
									exit(66);
								}

								sleep(5);
							}

							free(scwc);
							free(detrimis);
						}
					}
				}
			}
		}
		else {
			if (receive == 0 || strcmp(cBuffer, "stop") == 0) {
				sprintf(cBuffer, "%s has disconnected\n", cli -> Nume);
				printf("%s", cBuffer);
				// nu mai tr msj la client ca s-a deconectat, isi tr singur
				disconnectClient = 1;
			}
			else {
				printf("ERROR: -1\n");
				disconnectClient = 1;
			}
		}
	}

	close(cli -> sockfd);
	removeFromClientsList(cli -> uid);
	free(cli);

	nrClients--;

	pthread_detach(pthread_self());
	return NULL;
}

void WaitForOtherClients() {

	pthread_t threadID;
	struct sockaddr_in client_addr;
	int connfd = 0;

	while(1) {
		socklen_t clientLen = sizeof(client_addr);
		if((connfd = accept(ServerSocket, (struct sockaddr*) &client_addr, &clientLen)) == -1) {
			printf("Accepted failed in WaitForOtherClient()\n");
		}
		else {
			// creeare client

			ClientData *cli = (ClientData *) malloc(sizeof(ClientData));
			cli -> address = &client_addr;
			cli -> sockfd = connfd;
			cli -> uid = uid++;
			cli -> Fisier = NULL;

			addToClientsList(cli);

			if(pthread_create(&threadID, NULL, &threadClient, (void *)cli) !=0 ){
				printf("Failed to create thread\n");
			}
		}

		sleep(1);
	}
}

void createConnection() {

	int option = 1;

	ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(port);
	ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	// ignore signals from pipes
	signal(SIGPIPE, SIG_IGN);

	if(setsockopt(ServerSocket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option)) < 0) {
		printf("Eroare la setsockopt\n");
		exit(281);
	}

	if(bind(ServerSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)) < 0) {
		printf("Eroare la socket binding\n");
		exit(282);
	}

	if(listen(ServerSocket, 10) < 0) {
		printf("Eroare la socket listening\n");
		exit(283);
	}

	WaitForOtherClients();
}

int main() {

	checkdir();
	createConnection();

	return 0;
}

