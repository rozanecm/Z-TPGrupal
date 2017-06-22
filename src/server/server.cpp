//
// Created by rodian on 27/05/17.
//

#include "server.h"

Server::Server(unsigned int port, Menu &menu) : socket(port),
                                                running(true),
                                                menu(menu),
                                                port(port)
{}

void Server::run() {
    std::cout << "Server started on port " << port << std::endl;
    try {
        int i = 0;
        while(this->running) {
            Socket new_client = this->socket.accept_client();
            Messenger* messenger = new Messenger(new_client);
            std::string id_new_player =  messenger->recieveMessage();

            bool added = menu.addPlayer(messenger, menu,id_new_player);
            if (!added) {
                messenger->sendMessage("error");
                messenger->shutdown();
            }
            ++i;
            messenger->sendMessage("ok");
            std::cout << "new player conectado" << std::endl;

        }
    } catch (SocketError& e) {
        std::string error = e.what();
//        PrintStdErr printer(error);
    }
}

void Server::stop() {
    this->running = false;
    this->socket.shutdown();
}

Server::~Server() {}
