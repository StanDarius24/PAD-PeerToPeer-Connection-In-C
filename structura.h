#ifndef STRUCTURA_H
#define STRUCTURA_H
typedef struct node{
    char NumeFisier[25];
    struct node *urm;
}node;
typedef struct ClientData{
    char Nume[25];
    struct node *Fisier;
}ClientData;
void parcurgereFisiere(node *Nod);
node * CreeateNode(char *fisier);
node * AddFile(node *Nod,char *filename);
ClientData *CreereClient(char *name);
ClientData *AdaugareFisier(ClientData *Client,char *numeFisier);
void PrintareClient(ClientData *Client);
#endif