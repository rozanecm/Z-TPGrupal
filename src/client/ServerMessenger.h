#ifndef Z_TPGRUPAL_SERVERMESSENGER_H
#define Z_TPGRUPAL_SERVERMESSENGER_H

#include <mutex>
#include <string>
#include "../common/socket.h"

/* Handles communication with the server */
class ServerMessenger {
    Socket s;
    std::mutex m;
public:
    /* Constructor: connects to the given addr/port combination */
    ServerMessenger(const char* addr, unsigned int port);

    void send(const std::string& message);
    std::string receive();

    void kill();
};


#endif //Z_TPGRUPAL_SERVERMESSENGER_H
