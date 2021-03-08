#include "structura.h"

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<string.h>
#include <dirent.h>
#include <limits.h>



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
    if(Nod!=NULL)
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

void PrintareAllFiles(node *allFiles)
{
    if(allFiles != NULL)
    {
        parcurgereFisiere(allFiles);
        printf("\n");
    } else
            printf("0 files found\n");
}

int FileAlreadyExistsInAllFiles(node *allFiles, char *filename)
{
    if(allFiles == NULL)
        return 0;
    while(allFiles != NULL)
    {
        if(strcmp(allFiles->NumeFisier, filename) == 0)
            return 1;
        allFiles = allFiles -> urm;
    }
        return 0;
}

node *AddToAllFiles(node *allFiles, char *filename)
{
    if(allFiles == NULL)
    {
        struct node *nodInt = malloc(sizeof(struct node));
        strcpy(nodInt->NumeFisier, filename);
        allFiles = nodInt;
        allFiles->urm = NULL;
        return allFiles;
    } 

    struct node *nodInt = malloc(sizeof(struct node));
    strcpy(nodInt->NumeFisier, filename);
    nodInt->urm = NULL;
    struct node *nodParcurge = allFiles;

    while(nodParcurge->urm != NULL)
        nodParcurge = nodParcurge->urm;
    nodParcurge->urm = nodInt;
    return allFiles;
}