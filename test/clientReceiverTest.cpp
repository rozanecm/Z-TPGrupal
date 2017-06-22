//
// Created by rodian on 15/06/17.
//

#include "clientReceiverTest.h"

clientReceiverTest::clientReceiverTest(Messenger &messenger, std::mutex &m) :
     m(m),messenger(messenger), listen(true) {}

void clientReceiverTest::run() {
    while (listen) {
        std::string msg = messenger.recieveMessage();

//        m.lock();
        std::cerr << "Answer: " << msg << std::endl;
//        m.unlock();
    }
}
