#ifndef Z_TPGRUPAL_UNIT_H
#define Z_TPGRUPAL_UNIT_H


#include <vector>
#include <string>
#include <map>
#include "gtkmm/drawingarea.h"
#include "Armament.h"
#include "enums/TeamEnum.h"
#include "enums/ActionsEnum.h"
#include "enums/UnitsEnum.h"
#include "enums/RotationsEnum.h"
#include <utility>
#include <mutex>

class Unit {
private:
    int id;

    /* unitType can be: robot, vehicle, tank */
    UnitsEnum unitType;

    std::string unit_name;

    ActionsEnum actionType;

    RotationsEnum rotation;

    Armament armament;

    /* freq. with which the unit shoots */
    unsigned short frequency;

    unsigned int lifeLeft;

    unsigned int totalLife;

    unsigned short velocity;

    std::pair<unsigned int, unsigned int> position;
    std::pair<unsigned int, unsigned int> prev_position;

    /* bool selected: indicates weather the unit has been selected
     * with the mouse or not */
    bool selected;

    /* bool that indicates whether the unit is shooting or not */
    bool shooting;

    /* indicates to which getTeam the unit belongs */
    TeamEnum team;

    /* counters to know which img of the ones that conform an animation
     * should be drawn*/
    unsigned short shootingDrawingCounter;
    unsigned short standingDrawingCounter;
    unsigned short walkingDrawingCounter;

    std::string owner;
public:
    Unit(int id, std::pair<unsigned int, unsigned int> position,
             UnitsEnum unitType, TeamEnum team, unsigned int hp);

    Unit();
    int get_ID();

    void update_position(int x, int y);

    void update_state(ActionsEnum state);

    void markAsSelectedInRange(bool &unitsSelected, gdouble xStartCoordinate,
                               gdouble yStartCoordinate,
                               gdouble xFinishCoordinate,
                               gdouble yFinishCoordinate);

    bool isShooting();

    TeamEnum getTeam();

    RotationsEnum getRotation();

    UnitsEnum getType();

    ActionsEnum getAction();

    unsigned int getXCoordinate();

    unsigned int getYCoordinate();

    bool is_selected();

    void unselect();

    void update_rotation();

    void update_owner(const std::string& owner);

    void update_unit_name(const std::string& name);

    void update_hp(unsigned int hp);
    std::string get_owner();
    int get_max_hp();
    int get_hp();
    std::string get_unit_name();
};


#endif //Z_TPGRUPAL_UNIT_H
