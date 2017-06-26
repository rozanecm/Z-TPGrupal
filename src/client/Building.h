#ifndef Z_TPGRUPAL_BUILDING_H
#define Z_TPGRUPAL_BUILDING_H

#include <gtkmm/drawingarea.h>
#include "Player.h"
#include "enums/BuildingsEnum.h"
#include <utility>
#include <map>
#include <string>
#include <vector>

class Building {
private:
    BuildingsEnum buildingType;
    TeamEnum team;
    std::pair<unsigned int, unsigned int> position;

    /* bool selected: indicates wether the unit has been selected
     * with the mouse or not */
    bool selected;
    int id;
    std::string owner;
    int minutes;
    int seconds;
    unsigned int max_hp;
    unsigned int hp;
public:
    Building(BuildingsEnum type, int x, int y, int id, TeamEnum team,
             const std::string& owner, unsigned int hp);

    Building();
    /**
     * This methods checks all the player's units to see if any of its units is
     * located within the area of selection. If so, the units' attribute
     * selected is set to TRUE.
     */
    void markAsSelectedInRange(bool &buildingSelected,
                               gdouble xStartCoordinate,
                               gdouble yStartCoordinate,
                               gdouble xFinishCoordinate,
                               gdouble yFinishCoordinate);

    unsigned int getXCoordinate();

    unsigned int getYCoordinate();

    BuildingsEnum getBuildingType();

    TeamEnum getTeam();
    bool is_selected();
    void unselect();
    std::string get_owner();
    unsigned int get_hp();

    unsigned int get_max_hp();

    int get_ID();

    void update_time_left(int minutes, int seconds);

    void update_hp(unsigned int hp);

    void update_team(TeamEnum team);

    std::pair<int, int> get_time_left();

    void destroy();
};


#endif //Z_TPGRUPAL_BUILDING_H
