//
// Created by rodian on 27/05/17.
//

#ifndef Z_TPGRUPAL_SERVER_H
#define Z_TPGRUPAL_SERVER_H


#include "../common/socket.h"
#include "../common/messenger.h"
#include "../common/Thread.h"
#include "menu.h"

class Server: public Thread  {
private:
    int port;
    Socket socket;
    bool running;
    Menu& menu;

public:
    // Recieves the arguments to build the Control Unit
    explicit Server(unsigned int port, Menu &menu);

    // use to start the process of of accepting clients
    void run();

    // Shuts down the accepter Socket and stops run()
    void stop();

    ~Server();
};


#endif //Z_TPGRUPAL_SERVER_H
