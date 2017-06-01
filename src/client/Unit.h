#ifndef Z_TPGRUPAL_UNIT_H
#define Z_TPGRUPAL_UNIT_H


#include <vector>
#include <string>
#include <map>
#include "gtkmm/drawingarea.h"
#include "Armament.h"
#include <utility>

class Unit {
public:
    int get_ID();

    void update_position(int x, int y);

    void markAsSelectedInRange(gdouble xStartCoordinate,
                               gdouble yStartCoordinate,
                               gdouble xFinishCoordinate,
                               gdouble yFinishCoordinate);

private:
    int id;

    Armament armament;

    unsigned short frequency;

    unsigned int lifeLeft;

    unsigned int totalLife;

    /* animations are stored in a map where the key indicates the kind
     * of animations being stored and the value stores the images needed
     * to draw mentioned animation. */
    std::map<std::string, std::vector<Glib::RefPtr<Gdk::Pixbuf>>> animations;

    unsigned short velocity;

    std::pair<int, int> position;
    std::pair<int, int> prev_position;

    /* bool selected: indicates wether the unit has been selected
     * with the mouse or not */
    bool selected;
};


#endif //Z_TPGRUPAL_UNIT_H
