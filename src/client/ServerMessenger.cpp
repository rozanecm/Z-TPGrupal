
#include <string>
#include "ServerMessenger.h"
#include "../common/Lock.h"
#include "../common/messenger.h"
#include "../common/socket.h"

ServerMessenger::ServerMessenger(const char *addr, unsigned int port) :
    s(addr, port)
{
}

void ServerMessenger::send(const std::string &message) {
    Lock l(m);
    protocol_send(s, message.c_str(), (unsigned int) message.size());
}

std::string ServerMessenger::receive() {
    Lock l(m);
    return protocol_receive(s);
}

void ServerMessenger::kill() {
    s.shutdown();
}
