#ifndef Z_TPGRUPAL_GAMEBUILDER_H
#define Z_TPGRUPAL_GAMEBUILDER_H

#include <gtkmm.h>

#include "GameWindow.h"
class GameBuilder {
    GameWindow* window;

    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Image* portrait;
public:
    GameBuilder();
    ~GameBuilder();

    // returns the generated window
    GameWindow* get_window();

private:
};


#endif //Z_TPGRUPAL_GAMEWINDOW_H
