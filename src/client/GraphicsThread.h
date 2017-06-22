#ifndef Z_TPGRUPAL_GRAPHICSTHREAD_H
#define Z_TPGRUPAL_GRAPHICSTHREAD_H

#include <Thread.h>
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "ServerMessenger.h"
#include "UnitsMonitor.h"
#include "windows/GameWindow.h"

class GraphicsThread : public Thread {
    UnitsMonitor &unitsMonitor;
    BuildingsMonitor &buildingsMonitor;
    MapMonitor &mapMonitor;
    ServerMessenger &messenger;
    GameWindow &window;
    const std::string &owner;
public:
    GraphicsThread(UnitsMonitor &monitor,
                   BuildingsMonitor &buildingsMonitor,
                   MapMonitor &mapMonitor,
                   ServerMessenger &messenger,
                   GameWindow &window, const std::string &owner);

    virtual void run();
};


#endif //Z_TPGRUPAL_GRAPHICSTHREAD_H
