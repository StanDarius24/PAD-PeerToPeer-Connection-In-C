chmod 700 server.c
chmod 700 structura.c
chmod 700 structura.h
gcc -c structura.c
chmod 700 disc.c
chmod 700 disc.c
gcc -c disc.c
gcc -Wall -D_REENTRANT -pthread -o sv server.c structura.o disc.o
./sv