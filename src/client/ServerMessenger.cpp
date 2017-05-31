
#include <string>
#include "ServerMessenger.h"
#include "../common/Lock.h"
#include "../common/messenger.h"
#include "../common/socket.h"

ServerMessenger::ServerMessenger(Socket& s) :
    messenger(s) {
}

void ServerMessenger::send(const std::string &message) {
    Lock l(m);
    messenger.sendMessage(message);
}

std::string ServerMessenger::receive() {
    Lock l(m);
    return messenger.recieveMessage();
}

void ServerMessenger::kill() {
    messenger.shutdown();
}
