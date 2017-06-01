#ifndef Z_TPGRUPAL_BUILDING_H
#define Z_TPGRUPAL_BUILDING_H

#include <gtkmm/drawingarea.h>
#include "Player.h"
#include <utility>
#include <map>
#include <string>
#include <vector>

class Building {
private:
    /* animations are stored in a map where the key indicates the kind
     * of animations being stored and the value stores the images needed
     * to draw mentioned animation. */
    std::map<std::string, std::vector<Glib::RefPtr<Gdk::Pixbuf>>> animations;

    /* units that can be created are stored as a pair, where the int
     * indicates time needed and the Glib::RefPtr<Gdk::Pixbuf>
     * stores an image of the unit */
    std::vector<std::pair<int, Glib::RefPtr<Gdk::Pixbuf>>>
            unitsThatCanBeCreated;

    Player* owner;

    std::pair<int, int> position;

    /* bool selected: indicates wether the unit has been selected
     * with the mouse or not */
    bool selected;

public:
    /**
     * This methods checks all the player's units to see if any of its units is
     * located within the area of selection. If so, the units' attribute
     * selected is set to TRUE.
     */
    void markAsSelectedInRange(gdouble xStartCoordinate,
                               gdouble yStartCoordinate,
                               gdouble xFinishCoordinate,
                               gdouble yFinishCoordinate);
};


#endif //Z_TPGRUPAL_BUILDING_H
