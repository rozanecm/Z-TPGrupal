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

void PlayersMonitor::markAsSelectedInRange(gdouble xStartCoordinate,
                                           gdouble yStartCoordinate,
                                           gdouble xFinishCoordinate,
                                           gdouble yFinishCoordinate) {
    Lock l(m);
    for (Player& player : players){
        /* each player has to selects its units in range */
        player.markAsSelectedInRange(xStartCoordinate, yStartCoordinate,
                                     xFinishCoordinate, yFinishCoordinate);
    }
}
