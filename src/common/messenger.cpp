//
// Created by rodian on 22/05/17.
//

#include "messenger.h"

Messenger::Messenger(Socket& socket) : socket(std::move(socket)),
                                       connected(true) {}

std::string Messenger::recieveMessage() {
    std::string message = "remote socket finish";
    if ((socket.isValid()) && (this->connected)){
        // recieve length of message
        int status = -2;
        uint32_t len = 0;
        status = socket.socketRecieve((char*) &len, (size_t)sizeof(len));
        if (status == 0)
            this->connected = false;

        int lenght = ntohl(len);

        // recieve message
        if ((lenght > 0) && (this->connected)) {
            char *buff = new char[lenght];
            status = 0;

            status = socket.socketRecieve(buff, (size_t)lenght);
            if (status == 0)
                this->connected = false;

            std::string received_message(buff);
            message = std::move(received_message);
            delete [] buff;
        }
    }
    return message;
}

void Messenger::sendMessage(std::string& message) {
    if (this->connected) {
        // Send length first
        uint32_t network_len = htonl((uint32_t) (message.size() + 1));

        int sent = this->socket.socketSend((char*) &network_len,
                                           (size_t)sizeof(network_len));

        if (sent <= 0){
            this->connected = false;
        } else {
            // Send message
            const char* msg = message.c_str();
            this->socket.socketSend(msg, message.size() + 1);
        }
    }
}

bool Messenger::isConnected() {
    return this->connected;
}

void Messenger::shutdown() {
    this->socket.socketShutDownForRead();
    this->socket.socketShutDownForWrite();
}

Messenger::~Messenger() {}
