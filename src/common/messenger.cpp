#include <netinet/in.h>
#include <string>
#include "socket.h"
#include "messenger.h"

Messenger::Messenger(Socket& socket) : socket(std::move(socket)),
                                       connected(true) {}

#define MSG_SIZE 1024

std::string Messenger::recieveMessage() {
    // Receive length first, then the message
    uint32_t len = 0;
    socket.receive((char*) &len, sizeof(len));
    len = ntohl(len);
    char* buf = new char[len];
//    memset(buf, 0, len);
    ssize_t sent = socket.receive(buf, len);
    if (sent <= 0) {
        throw(SocketError("Socket closed"));
    }
    std::string result(buf);

//    std::string asdf = std::move(result);
    delete[] buf;
    return result;
}

void Messenger::sendMessage(const std::string &message) {
    if (this->connected) {
        uint32_t len = (uint32_t) message.size() + 1;
        // Send length first, then the message
        uint32_t network_len = htonl(len);
        socket.send((char *) &network_len, sizeof(network_len));
        socket.send(message.c_str(), len);
    }
}

void Messenger::shutdown() {
    socket.shutdown();
}

bool Messenger::isConnected() {
    return socket.is_valid();
}

Messenger::~Messenger() {
}
