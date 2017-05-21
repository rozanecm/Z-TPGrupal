#ifndef Z_TPGRUPAL_GRAPHICSTHREAD_H
#define Z_TPGRUPAL_GRAPHICSTHREAD_H

#include "Thread.h"
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"

class GraphicsThread : public Thread{
public:
    GraphicsThread(int argc, char **argv, PlayersMonitor &monitor,
                   BuildingsMonitor &buildingsMonitor);

    virtual void run();

private:
    int argc;

    char **argv;

    PlayersMonitor &playerMonitor;

    BuildingsMonitor &buildingsMonitor;
};


#endif //Z_TPGRUPAL_GRAPHICSTHREAD_H
