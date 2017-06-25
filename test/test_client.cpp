#include "socket.h"
#include "messenger.h"
#include <mutex>
#include <iostream>
#include "clientReceiverTest.h"

int main() {
    Socket s("127.0.0.1", 8000);
    Messenger m(s);
    std::mutex mutex;
    clientReceiverTest client(m,mutex);
    client.start();

    std::string msg0 = "player1";
    m.sendMessage(msg0);

//    std::string msg1 = "createlobby";
//    m.sendMessage(msg1);
//
//
//    msg1 = "ready";
//    m.sendMessage(msg1);
//
//
//    msg1 = "startgame";
//    m.sendMessage(msg1);

    while(true) {
        std::cout << "Enter message: " << std::endl << ">> ";
        std::string msg;
        std::getline(std::cin, msg);
        m.sendMessage(msg);
    }
}