
#include <string>
#include "ServerMessenger.h"
#include "../common/Lock.h"
#include "../common/messenger.h"
#include "../common/socket.h"

ServerMessenger::ServerMessenger(Socket& s) :
    messenger(s) {
}

void ServerMessenger::send(const std::string &message) {
    Lock l(send_m);
    messenger.sendMessage(message);
}

std::string ServerMessenger::receive() {
    Lock l(recv_m);
    return messenger.recieveMessage();
}

void ServerMessenger::kill() {
    messenger.shutdown();
}
