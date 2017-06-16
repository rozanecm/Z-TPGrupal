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
            std::string id_new_player = "Player" + std::to_string(i);
            menu.addPlayer(messenger, menu,id_new_player);
            std::cout << "new player conectado" << std::endl;
//              create player with messenger
//            std::string logIn_msg = messenger->recieveMessage();
//            processMessage(logIn_msg, messenger);
//
//           for (std::vector<UserOperator*>::iterator it = this->users.begin();
//                     it != this->users.end(); ) {
//                    if ((*it)->finished()) {
//                        (*it)->join();
//                        delete(*it);
//                it = this->users.erase(it);//erase gives me the next iterator
//                    } else { ++it; }
//                }

        }
    } catch (SocketError& e) {
        std::string error = e.what();
//        PrintStdErr printer(error);
    }
}

bool Server::processMessage(std::string& logIn_msg, Messenger* msgr) {
    std::string user = getNextData(logIn_msg);
    bool no_problem = true;
    if (user == "alumno") {
//        std::string id_str = getNextData(logIn_msg);
//        int id = std::stoi(id_str);
//        if (this->checkStudentId(id, user)) {
//            StudentOperator* student = new StudentOperator(id, control, msgr);
//            this->users.emplace_back(student);
//            // Make user to start recieving commands from client
//            this->users.back()->start();
//
//        } else { no_problem = false;}
//    } else if (user == "docente") {
//        std::string id_str = getNextData(logIn_msg);
//        int id = std::stoi(id_str);
//        if (this->checkProfessorId(id, user)) {
//      ProfessorOperator* professor = new ProfessorOperator(id, control, msgr);
//            this->users.emplace_back(professor);
//            // Make user to start recieving commands from client
//            this->users.back()->start();
//
//        } else { no_problem = false;}
//    } else if (user == "admin") {
//        Admin* admin = new Admin(this->control, msgr);
//        this->users.emplace_back(admin);
//        // Make user to start recieving commands from client
//        this->users.back()->start();

    } else {
        std::string msg_error = user;
        msg_error += " es un tipo de usuario inválido.\n";
//        PrintStdErr printer(msg_error);
        no_problem = false;
    }
    return no_problem;
}

std::string Server::getNextData(std::string& line) {
    std::size_t found = line.find('-');
    std::string data = line.substr(0,found);
    line.erase(0,found+1);
    return data;
}

void Server::stop() {
    this->socket.shutdown();
    this->running = false;
}

Server::~Server() {
//    delete(this->control);

//    for (std::vector<Player*>::iterator it = this->users.begin();
//         it != this->users.end(); ) {
//        (*it)->stop();
//        (*it)->join();
//        delete(*it);
//        it = this->users.erase(it);//erase gives me the next iterator
//    }
}
