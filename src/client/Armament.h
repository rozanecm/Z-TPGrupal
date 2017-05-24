#ifndef Z_TPGRUPAL_ARMAMENT_H
#define Z_TPGRUPAL_ARMAMENT_H


#include <vector>
#include <string>
#include <map>
#include <gtkmm/drawingarea.h>
#include <SDL2/SDL_mixer.h>

class Armament {
private:
    /* sound used when shot */
    Mix_Chunk* sound;

    /* animation is stored as a series of images needed
     * to draw the animation. */
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> animation;
};


#endif //Z_TPGRUPAL_ARMAMENT_H
