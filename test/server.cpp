#include "socket.h"
#include "messenger.h"
#include <iostream>

int main() {
    Socket s(8000);
    Socket client = s.accept_client();
    std::cout << "Client accepted" << std::endl;
    std::string msg = "move-1-12-200";
    protocol_send(client, msg.c_str(), msg.size());
    return 0;
}