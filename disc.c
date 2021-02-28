#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
       #include <fcntl.h>

#include "disc.h"
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
int main()
{	checkdir();
	return 0;
}