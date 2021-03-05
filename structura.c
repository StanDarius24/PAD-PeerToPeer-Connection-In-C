#include "structura.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>



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
