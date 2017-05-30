#include <netinet/in.h>
#include <string>
#include "socket.h"
#include "messenger.h"

Messenger::Messenger(Socket& socket) : socket(std::move(socket)),
                                       connected(true) {}

std::string Messenger::recieveMessage() {
    // Receive length first, then the message
    uint32_t len = 0;
    socket.receive((char*) &len, sizeof(len));
    len = ntohl(len);
    char buf[MSG_SIZE] = "";
    socket.receive(buf, len);
    std::string result(buf);
    return result;
}

void Messenger::sendMessage(std::string& message) {
    if (this->connected) {
        // Send length first
        uint32_t network_len = htonl((uint32_t) (message.size() + 1));

        int sent = this->socket.socketSend((char*) &network_len,
                                           (size_t)sizeof(network_len));
=======
#define MSG_SIZE 1024
>>>>>>> origin/client-socket

void protocol_send(Socket& s, const char* msg, unsigned int len) {
    // Send length first, then the message
    uint32_t network_len = htonl(len);
    s.send((char*) &network_len, sizeof(network_len));
    s.send(msg, len);
}

std::string protocol_receive(Socket& s) {
}
