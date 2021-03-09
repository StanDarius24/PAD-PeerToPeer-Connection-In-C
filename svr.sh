chmod 700 server.c
chmod 700 structura.c
chmod 700 structura.h
gcc -c structura.c
gcc -Wall -D_REENTRANT -pthread -o sv server.c structura.o 
./sv