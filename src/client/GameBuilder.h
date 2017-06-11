#ifndef Z_TPGRUPAL_GAMEBUILDER_H
#define Z_TPGRUPAL_GAMEBUILDER_H

#include <gtkmm.h>
#include <string>

#include "GameWindow.h"
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "ServerMessenger.h"
#include "windows/InitialWindow.h"

class GameBuilder {
    GameWindow* window;
    InitialWindow* init_window;
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Image* portrait;
    Gtk::Box* panel;
    Gtk::Box* unit_panel;
    Gtk::Box* building_panel;
    Gtk::Box* group_panel;
    Gtk::Button* button;

public:
    GameBuilder();
    ~GameBuilder();

    // returns the generated window
    GameWindow* get_window();
    InitialWindow* get_initial_window();
private:
};


#endif //Z_TPGRUPAL_GAMEWINDOW_H
