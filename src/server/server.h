//
// Created by rodian on 27/05/17.
//

#ifndef Z_TPGRUPAL_SERVER_H
#define Z_TPGRUPAL_SERVER_H


#include "../socket.h"
#include "../messenger.h"
#include "../Thread.h"

class Server: public Thread  {
private:
    Socket socket;
    bool running;
    //std::vector<Player*> users;

public:
    // Recieves the arguments to build the Control Unit
    explicit Server(char* argv[]);

    // use to start the process of of accepting clients
    void run();

    // Shuts down the accepter Socket and stops run()
    void stop();

    ~Server();

private:
    // Process the logIn message to know which kind
    // of user is logIn
    bool processMessage(std::string& message,  Messenger* msgr);

    // Returns the next word of the command received
    // reducing the size of the string with the command given
    std::string getNextData(std::string& line);

};


#endif //Z_TPGRUPAL_SERVER_H
