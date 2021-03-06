//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_GAME_H
#define Z_TPGRUPAL_GAME_H


#include "map.h"
#include "controlUnit.h"
#include "player.h"
#include "commandMonitor.h"

class ControlUnit;
class Player;

class Game: public Thread {
private:
    std::mutex m;
    CommandMonitor commands;
    std::map<int,Unit*> all_units;
    std::vector<Player*> players;
    std::vector<Occupant*> all_occupants;
    std::vector<Territory*> territories;
    std::vector<Team> teams;
    ControlUnit* control;
    std::shared_ptr<Map> map;
    std::string path, config;
    std::map<std::string,std::string> types;
    std::vector<UnitMold*> unit_molds;
    bool finished;

public:
//    Game(std::vector<Player *> players, std::vector<Messenger *> msgr,
//             std::shared_ptr<Map> map, std::map<int, Unit *> units,
//             std::vector<Team>& teams_info, std::vector<Occupant *> occupants,
//             std::vector<Territory *> &territories);

    Game(std::string path, std::string &config, std::vector<Team> &teams_info,
             std::vector<Player *> &players);

    void run();

    void shutDownGame();

    void sendMapInfo(ControlUnit &control);

    void sendOccupantsInfo();

    bool gameHaveFinished();

    void disconectPlayer(std::string player);

    ~Game();
private:
    void analyseOccupantsInfo(std::string& info);

    void buildTypeMap();

    void sincronizeOccupants();

    void sendTerritoryInfo();

    void createStartingUnits();

    void buildMap();

    std::vector<Messenger*> getMessengers();
};


#endif //Z_TPGRUPAL_GAME_H
