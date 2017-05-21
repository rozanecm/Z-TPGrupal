#ifndef Z_TPGRUPAL_GRAPHICSTHREAD_H
#define Z_TPGRUPAL_GRAPHICSTHREAD_H

#include "Thread.h"

class GraphicsThread : public Thread{
public:
    GraphicsThread(int argc, char **argv);

    virtual void run();

private:
    int argc;
    char **argv;
};


#endif //Z_TPGRUPAL_GRAPHICSTHREAD_H
