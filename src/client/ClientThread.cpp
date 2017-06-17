#include <iostream>
#include <string>
#include <vector>
#include <pugixml.hpp>
#include <split.h>
#include <utility>
#include "ClientThread.h"
#include "ServerMessenger.h"
#include "commands/Command.h"
#include "commands/AddUnit.h"
#include "commands/RemoveUnit.h"
#include "commands/UpdatePosition.h"
#include "commands/LoadMap.h"
#include "commands/Update.h"
#include "windows/GameWindow.h"
#include "commands/FactoryNextUnit.h"

void ClientThread::run() {
    initCommands();
    loop();
}

ClientThread::ClientThread(UnitsMonitor &unitsMonitor,
                           BuildingsMonitor &buildingsMonitor,
                           MapMonitor &mapMonitor,
                           ServerMessenger &messenger,
                           GameWindow& window
) :
        unitsMonitor(unitsMonitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor),
        messenger(messenger),
        window(window)
{
}

void ClientThread::loop() {
    try {
        while (!finished) {
            std::string msg = messenger.receive();
            parse(msg);
        }
    } catch(SocketError& e) {
        return;
    }
}

void ClientThread::parse(std::string &s) {
    std::vector<std::string> params = split(s, '-');
    int cmd = 0;
    auto result = commands.find(params[cmd]);
    if (result == commands.end()) {
        std::cerr << "Invalid command: " << params[cmd] << std::endl;
        return;
    }
    std::vector<std::string> args(++params.begin(), params.end());
    std::cout << "Executing " << params[cmd] << std::endl;
    result->second->execute(args);
}

void ClientThread::finish() {
    finished = true;
    messenger.kill();
}

void ClientThread::initCommands() {
    commands["loadmap"] = new LoadMap(mapMonitor);
    commands["addunit"] = new AddUnit(unitsMonitor);
    commands["removeunit"] = new RemoveUnit(unitsMonitor);
    commands["move"] = new UpdatePosition(unitsMonitor);
    commands["update"] = new Update();
    commands["nextunit"] = new FactoryNextUnit(window);
}

ClientThread::~ClientThread() {
    for (std::pair<std::string, Command*> c : commands) {
        delete c.second;
    }
}
