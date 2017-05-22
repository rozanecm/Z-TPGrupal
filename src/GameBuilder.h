#ifndef Z_TPGRUPAL_GAMEBUILDER_H
#define Z_TPGRUPAL_GAMEBUILDER_H

#include <gtkmm.h>

#include "GameWindow.h"
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"

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
public:
    GameBuilder(PlayersMonitor &monitor, BuildingsMonitor &buildingsMonitor,
                    MapMonitor &mapMonitor);
    ~GameBuilder();

    Gtk::Window* get_window();
    bool change_view_to_unit();
    bool change_view_to_building();
    bool change_view_to_unit_group();

private:
    void update_portrait(const std::string& img_path) const;
};


#endif //Z_TPGRUPAL_GAMEWINDOW_H
