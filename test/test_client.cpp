#include "socket.h"
#include "messenger.h"
#include <iostream>

int main() {
    Socket s("127.0.0.1", 8000);
    Messenger m(s);
    std::string msg1 = "create-lobby";
    m.sendMessage(msg1);
    std::cout << "Response: " << m.recieveMessage() << std::endl;

    msg1 = "ready";
    m.sendMessage(msg1);
    std::cout << "Response: " << m.recieveMessage() << std::endl;

    msg1 = "start-game";
    m.sendMessage(msg1);
    std::cout << "Response: " << m.recieveMessage() << std::endl;
    while(true) {
        std::cout << "Enter message: " << std::endl << ">> ";
        std::string msg;
        std::getline(std::cin, msg);
        m.sendMessage(msg);
        std::cout << "Response: " << m.recieveMessage() << std::endl;
    }
}