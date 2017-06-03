#ifndef Z_TPGRUPAL_GRAPHICSTHREAD_H
#define Z_TPGRUPAL_GRAPHICSTHREAD_H

#include <Thread.h>
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "ServerMessenger.h"

class GraphicsThread : public Thread {

    PlayersMonitor &playerMonitor;
    BuildingsMonitor &buildingsMonitor;
    MapMonitor &mapMonitor;
    ServerMessenger &messenger;
public:
    GraphicsThread(PlayersMonitor &monitor,
                   BuildingsMonitor &buildingsMonitor,
                   MapMonitor &mapMonitor,
                   ServerMessenger &messenger);

    virtual void run();

private:

};


#endif //Z_TPGRUPAL_GRAPHICSTHREAD_H
