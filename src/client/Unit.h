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
public:
    int get_ID();

    void update_position(int x, int y);

    void markAsSelectedInRange(gdouble xStartCoordinate,
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

private:
    int id;

    /* unitType can be: robot, vehicle, tank */
    UnitsEnum unitType;

    RotationsEnum rotation;

    Armament armament;

    unsigned short frequency;

    unsigned int lifeLeft;

    unsigned int totalLife;

    //todo this probably shouldn't exist.
    /* animations are stored in a map where the key indicates the kind
     * of animations being stored and the value stores the images needed
     * to draw mentioned animation. */
    std::map<std::string, std::vector<Glib::RefPtr<Gdk::Pixbuf>>> animations;

    unsigned short velocity;

    std::pair<unsigned int, unsigned int> position;
    std::pair<unsigned int, unsigned int> prev_position;

    /* bool selected: indicates wether the unit has been selected
     * with the mouse or not */
    bool selected;

    /* bool that indicates wheter the unit is shooting or not */
    bool shooting;

    /* indicates to which getTeam the unit belongs */
    TeamEnum team;

    /* counters to know which img of the ones that conform an animation
     * should be drawn*/
    unsigned short shootingDrawingCounter;
    unsigned short standingDrawingCounter;
    unsigned short walkingDrawingCounter;
};


#endif //Z_TPGRUPAL_UNIT_H
