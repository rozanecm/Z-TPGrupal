#include "PlayersMonitor.h"
#include "Player.h"
#include <vector>
#include <Lock.h>
#include <iostream>

PlayersMonitor::PlayersMonitor(std::vector<Player> &players) :
        players(players) {
}

void PlayersMonitor::addUnit(int player_id, Unit& unit) {
    Lock l(m);
    for (Player& player : players) {
        if (player.getID() == player_id) {
            player.addUnit(unit);
            return;
        }
    }
    std::cerr << "addUnit: Invalid player ID: " << player_id << std::endl;
}

void PlayersMonitor::removeUnit(int unit_id) {
    Lock l(m);
    for (Player& player : players) {
        if (player.removeUnit(unit_id)) {
            // True if unit_id was owned by that player, and removed
            return;
        }
    }
    std::cerr << "removeUnit: Invalid unit ID: " << unit_id << std::endl;
}


void PlayersMonitor::update_position(int unit_id, int x, int y) {
    Lock l(m);
    for (Player& player : players) {
        if (player.update_position(unit_id, x, y)) {
            // True if unit_id was owned by that player, and changed position
            return;
        }
    }
    std::cerr << "update_position: Invalid unit ID: " << unit_id << std::endl;
}

void PlayersMonitor::markAsSelectedInRange(bool &unitsSelected,
                                           gdouble xStartCoordinate,
                                           gdouble yStartCoordinate,
                                           gdouble xFinishCoordinate,
                                           gdouble yFinishCoordinate) {
    Lock l(m);
    for (Player& player : players){
        /* each player has to selects its units in range */
        player.markAsSelectedInRange(unitsSelected, xStartCoordinate,
                                     yStartCoordinate, xFinishCoordinate,
                                     yFinishCoordinate);
    }
}

std::vector<Unit> PlayersMonitor::getUnitsToDraw(unsigned int minX,
                                                         unsigned int maxX,
                                                         unsigned int minY,
                                                         unsigned int maxY) {
    Lock l(m);
    std::vector<Unit> returnVector;
    for (Player& player : players){
        /* each player has to selects its units in range */
        std::vector <Unit> currentPlayersVector = player.
                getUnitsToDraw(minX, maxX, minY, maxY);
        returnVector.insert(returnVector.end(), currentPlayersVector.begin(),
                            currentPlayersVector.end());
    }
    return returnVector;
}
