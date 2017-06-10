//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_MESSENGER_H
#define Z_TPGRUPAL_MESSENGER_H

#include "socket.h"
#include <iostream>
#include <string>
////////////////////////
// Messenger Class meant to use sockets
// to send messages between Client and
// Server using a specific protocol.
// Send lenght of message first, then the message.
////////////////////////
class Messenger{
private:
    Socket socket;
    bool connected;

public:
    // Recieves a unique socket to send
    // and recieves messages from
    explicit Messenger(Socket& socket);

    // Recieves a Message from the remote
    // connected socket.
    // Returns the message on a string
    std::string recieveMessage();

    // Sends a message to the remote socket
    // Recieves the message on a string
    void sendMessage(const std::string &message);

    // Shuts down the socket for read and write
    void shutdown();

    // If the sockets are still connected returns true
    // otherwise false.
    bool isConnected();

    ~Messenger();
};


#endif //Z_TPGRUPAL_MESSENGER_H
