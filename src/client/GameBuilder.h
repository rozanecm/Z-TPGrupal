#ifndef Z_TPGRUPAL_GAMEBUILDER_H
#define Z_TPGRUPAL_GAMEBUILDER_H

#include <gtkmm.h>
#include <string>

#include "GameWindow.h"
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "ServerMessenger.h"

class GameBuilder {
    GameWindow* window;

    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Image* portrait;
    Gtk::Box* panel;
    Gtk::Box* unit_panel;
    Gtk::Box* building_panel;
    Gtk::Box* group_panel;
    Gtk::Button* button;

    PlayersMonitor &playersMonitor;
    BuildingsMonitor &buildingsMonitor;
    MapMonitor &mapMonitor;
    ServerMessenger& messenger;

public:
    GameBuilder(PlayersMonitor &monitor, BuildingsMonitor &buildingsMonitor,
                    MapMonitor &mapMonitor, ServerMessenger& messenger);
    ~GameBuilder();

    // returns the generated window
    GameWindow* get_window();

private:
};


#endif //Z_TPGRUPAL_GAMEWINDOW_H
