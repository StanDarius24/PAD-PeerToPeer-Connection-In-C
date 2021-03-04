chmod 700 server.c
gcc -Wall -D_REENTRANT -pthread -o sv server.c structura.o disc.o
./sv