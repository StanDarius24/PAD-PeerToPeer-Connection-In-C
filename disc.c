#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include<string.h>
#include <dirent.h>
#include <limits.h>
#include "disc.h"

#include "structura.h"
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

void newUser(char *Name, struct ClientData * DataClient,int i)
{	int sw=1;
	DIR *director,*director2;
	struct dirent *dir,*dir2;
	struct stat info,info2;
	char *name = malloc(sizeof(char)*30);
	char *path = malloc(sizeof(char)*50);
	if(!(director=opendir("server")))
	{
		printf("Eroare la deschiderea directorului server\n");
		exit(9);
	}
	while((dir=readdir(director))>0 && sw)
	{
		strcpy(name,dir->d_name);
			if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0 )
					continue;
		snprintf(path,49,"server/%s",name);
	
		if(lstat(path,&info)<0)
		{
			printf("Eroare la functia stat\n");
			exit(10);
		}

		if(S_ISDIR(info.st_mode) && (strcmp(Name,name)==0))
		{
			printf("director existent, utilizator vechi\n");
			sw=0;
			if(!(director2=opendir(path)))
			{
				printf("Eroare la deschiderea directorului 2\n");
				exit(12);
			}
			char *names=malloc(sizeof(char)*50);
			char *path2=malloc(sizeof(char)*50);
			while((dir2=readdir(director2))>0)
			{
				strcpy(names,dir2->d_name);
				if(strcmp(names, ".") == 0 || strcmp(names, "..") == 0 )
					continue;
				snprintf(path2,49,"%s/%s",path,names);
				printf("path to files : %s \n",path2);

				if(lstat(path2,&info2)<0)
				{
					printf("Eroare la lstat in dir 2\n");
					exit(13);
				}
				if(S_ISREG(info2.st_mode))
				{
						printf("Adaugare \n");
					DataClient[i]=*AdaugareFisier(&DataClient[i],names);
				}


			}
			closedir(director2);
			free(names);
			free(path2);

		}
		
		if(sw==1)
		{
			printf("Utilizator nou, generam director!\n");

			pid_t child;

			if((child=fork())<0)
			{
				printf("Eroare la proces xD\n");
				exit(112);
			}else
			if(child==0)
			{
				char *xD=malloc(sizeof(char)*112);
				snprintf(xD,100,"server/%s",Name);
				execlp("mkdir","mkdir",xD,NULL);
			}


		}
		

	}
	closedir(director);
	free(name);
	free(path);
}
