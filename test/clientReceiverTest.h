//
// Created by rodian on 15/06/17.
//

#ifndef Z_TPGRUPAL_CLIENTRECEIVERTEST_H
#define Z_TPGRUPAL_CLIENTRECEIVERTEST_H


#include <mutex>
#include "messenger.h"
#include "Thread.h"

class clientReceiverTest: public Thread {
private:
    std::mutex &m;
    Messenger &messenger;
    bool listen;
public:
    clientReceiverTest(Messenger &messenger, std::mutex& m);

    void run();
};


#endif //Z_TPGRUPAL_CLIENTRECEIVERTEST_H
