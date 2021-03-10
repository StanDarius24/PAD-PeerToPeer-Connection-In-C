#ifndef STRUCTURA_H
#define STRUCTURA_H
#include <sys/socket.h>

typedef struct node{
    char NumeFisier[25];
    struct node *urm;
} node;

typedef struct ClientData{
	struct sockaddr_in *address;
	int sockfd;
	int uid;
    char Nume[32];
    struct node *Fisier;
} ClientData;

node *CreeateNode(char *fisier);
node * AddFile(node *Nod, char *filename);
void parcurgereFisiere(node *Nod);
void PrintareAllFiles(node *allFiles);
int FileAlreadyExistsInAllFiles(node *allFiles, char *filename);
node *AddToAllFiles(node *allFiles, char *filename);
void checkdir();

#endif
