#ifndef Z_TPGRUPAL_SERVERMESSENGER_H
#define Z_TPGRUPAL_SERVERMESSENGER_H

#include <mutex>
#include <string>
#include <messenger.h>
#include "../common/socket.h"

/* Handles communication with the server */
class ServerMessenger {
    Messenger messenger;
    std::mutex m;
public:
    /* Constructor: connects to the given addr/port combination */
    explicit ServerMessenger(Socket& s);

    void send(const std::string& message);
    std::string receive();

    void kill();
};


#endif //Z_TPGRUPAL_SERVERMESSENGER_H
