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
};


#endif //Z_TPGRUPAL_BUILDING_H
