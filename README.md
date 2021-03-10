# Peer-to-Peer File Transfer through Network

This project aims to develop a Peer-to-Peer File Transfer through a Network controlled and centralized by a Server.

# About

This is the first team project for our Distributed Applications Programming Laboratory, being an optional course for the 6th semester of Computer Science Bachelor's Degree at Politehnica Timisoara University.

# Team Members
* Stan Darius
* Sadovan Bogdan
* Roșca Nicoleta
* Sava Alexandru

# Description

* We define the client as a Peer who wants to download and in the same time share different files into the Network. After the client connected to the server, he may access a list of possible files to download through the server from other clients.
* The client sends a request to the server to download a file. In that moment, the server sends a request to all of the other peers to see where the exact file requested is located.
* Peers send back a message and the server decides how to split the file and creates the ports for future connections. Therefore, the server sends back to the initial client a list of ports where all the peer-to-peer connections will be established.
* Once that is made, the partial files will be partially transfered to the client and after completion a checksum is calculated and verified in order to proceed to the last step: file reconstruction.

# How to start the Server and Client

* You mush be running Linux/Ubuntu Terminal

* SERVER
* chmod 700 server.c
* chmod 700 structura.c
* chmod 700 structura.h
* gcc -c structura.c
* gcc -Wall -D_REENTRANT -pthread -o sv server.c structura.o
* ./sv

* CLIENT
* chmod 700 client.c
* gcc -c client.c
* gcc -Wall -D_REENTRANT -pthread -o cl client.c
* ./cl
