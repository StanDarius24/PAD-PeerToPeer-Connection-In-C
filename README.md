# Peer-to-Peer File Transfer through Network

# Team Members
* Stan Darius
* Sadovan Bogdan
* Roșca Nicoleta
* Sava Alexandru

# Description

* We define the client as a Peer who wants to download and in the same time share different files into the Network. After the client connected to the server, he may access a list of possible files to download through the server.
* The client sends a request to the server to download a file. In that moment, the server sends a request to all of the other peers to see where the exact file requested is located.
* Peers send back a message and the server decides how to split the file and creates the ports for future connections. Therefore, the server sends back to the initial client a list of ports where all the peer-to-peer connections will be established.
* Once that is made, the partial files will be partially transfered to the client and after completion a checksum is calculated and verified in order to proceed to the last step: file reconstruction.
