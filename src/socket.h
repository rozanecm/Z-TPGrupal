//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_SOCKET_H
#define Z_TPGRUPAL_SOCKET_H

#include "socketError.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

////////////////////////
// Socket Class
////////////////////////
class Socket {
private:
    int file_descriptor;
    struct addrinfo* result;
    struct addrinfo addr_pointer;

public:
    // Prepares the socket to be ready for use
    // for a client or a server
    // by argv[] knows if the builder is a server or a client
    explicit Socket(char* argv[]);

    // Uses the getaddressinfo fuction making the necesary settings
    // for a server or a client
    void setAddrInfo(char* argv[]);

    // Conects a client socket to the a server
    void connectToServer();

    // Creates a socket saving the file descriptor
    // On error it closes the program
    void createSocketTcp(struct addrinfo* addr_pointer);

    // Makes necesary preparations for server socket
    void setSocket();

    // Sends the batch of bytes to the previously connected
    // socket. On Error returns the retun of send
    // On success returns bytes sent
    int socketSend(const char* buff_to_send, size_t length);

    // Recieves the batch of bytes of the previously connected socket
    // On Error returns the return of recv
    // On success returns how many bytes recieved
    int socketRecieve(char* buff_to_receive, size_t length);

    // Binds and listen for a new socket
    void bindAndListen(struct addrinfo *addr_pointer);

    // Accepts the connection to a socket who wants
    // to connect with this one
    Socket socketAccept();

    // Shuts down the capability to recieve messages of the socket
    void socketShutDownForRead();

    // Shuts down the capability to send messages of the socket
    void socketShutDownForWrite();

    // Returns true if the file descriptor is valid
    // otherwise false
    bool isValid();

    Socket(Socket&& other);

    Socket(Socket&) = delete;

    void operator=(Socket&) = delete;

    ~Socket();

private:
    // Constructor meant to be use when a socket is accepted
    explicit Socket(int file_d);
};


#endif //Z_TPGRUPAL_SOCKET_H
