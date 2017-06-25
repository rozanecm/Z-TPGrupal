#include "MapMonitor.h"
#include <Lock.h>


void MapMonitor::setCell(unsigned int xCoordinate, unsigned int yCoordinate,
                         std::string terrainType) {
    Lock l(m);
    map.setCell(xCoordinate, yCoordinate, terrainType);
}

void MapMonitor::initializeMap(unsigned int xSize, unsigned int ySize) {
    Lock l(m);
    map.initializeMap(xSize, ySize);
}

unsigned int MapMonitor::getXSize() {
    Lock l(m);
    return map.getXSize();
}

unsigned int MapMonitor::getYSize() {
    Lock l(m);
    return map.getYSize();
}

std::string MapMonitor::getTerrainTypeAt(unsigned int xCoordinate,
                                         unsigned int yCoordinate) {
    Lock l(m);
    return map.getTerrainTypeAt(xCoordinate, yCoordinate);
}

void MapMonitor::markAsSelectedInRange(bool& terrainSelected, double
                                       xStartCoordinate,
                                       double yStartCoordinate,
                                       double xFinishCoordinate,
                                       double yFinishCoordinate) {
    //todo implementar esto que falta ver el tema de accidentales sobre el mapa.
}

void MapMonitor::addNature(Nature nature) {
    Lock l(m);
    map.addNature(nature);
}

std::vector<Nature>
MapMonitor::getNatureToDraw(unsigned int minX, unsigned int maxX,
                            unsigned int minY, unsigned int maxY) {
    Lock l(m);
    std::vector<Nature> returnVector;

    for (Nature &nature : map.getNature()) {
        if (nature.getPosition().first >= minX and
            nature.getPosition().first <= maxX and
            nature.getPosition().second >= minY and
            nature.getPosition().second <= maxY) {
            returnVector.emplace_back(nature);
        }
    }
    return returnVector;
}

void MapMonitor::update_players(const std::vector<std::string> &names) {
    Lock l(m);
    this->players = names;
}

int MapMonitor::get_player_id(const std::string &player) {
    int id = 1; // id 0 is neutral
    Lock l(m);
    auto it = players.begin();
    for (; it != players.end(); ++it) {
        if (*it == player) {
            break;
        }
        id++;
    }
    if (it == players.end()) {
        return 0;
    }
    return id; // Not found, return NEUTRAL id
}

void MapMonitor::finish_winner() {
    Lock l(m);
    winner = true;
}

void MapMonitor::finish_loser() {
    Lock l(m);
    loser = true;
}

bool MapMonitor::is_winner() {
    Lock l(m);
    return winner;
}

bool MapMonitor::is_loser() {
    Lock l(m);
    return loser;
}


void MapMonitor::clear() {
    Lock l(m);
    loser = false;
    winner = false;
    map.clear();
    players.clear();

}

void MapMonitor::update_territory(const int &id, const TeamEnum &team,
                                  const int &x, const int &y) {
    Lock l(m);
    map.update_territory(id, team, x, y);
}

std::map<int, std::pair<TeamEnum, std::pair<unsigned int, unsigned int>>>
MapMonitor::getFlags() {
    Lock l(m);
    return map.getFlags();
}
