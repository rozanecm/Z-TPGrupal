#ifndef Z_TPGRUPAL_GAMEWINDOW_H
#define Z_TPGRUPAL_GAMEWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/builder.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/applicationwindow.h>
#include "../GameArea.h"
#include "../BuildingsMonitor.h"
#include "../MapMonitor.h"
#include "../ServerMessenger.h"
#include "../panels/UnitPanel.h"
#include "../panels/BuildingPanel.h"

class GameWindow : public Gtk::ApplicationWindow {
    GameArea *gameArea;
    Gtk::Box *panel;
    UnitPanel *unit_panel;
    BuildingPanel *building_panel;
    Gtk::Box *group_panel;
    Gtk::Label *panelLabel;

    Gtk::Image *portrait;
    UnitsMonitor *unitsMonitor;
    BuildingsMonitor *buildingsMonitor;
    MapMonitor *mapMonitor;
    ServerMessenger *messenger;

    Unit selected_unit;
    Building selected_building;

    bool unit_selection;
    bool building_selection;
    int selection_id = 0;

    std::string me;
    std::vector<std::string> players;
protected:
    bool onTimeout();

public:
    GameWindow(BaseObjectType *cobject,
               const Glib::RefPtr<Gtk::Builder> &builder);

    virtual ~GameWindow();

    /**
     * Saves resources' monitors and passes them to the gameArea.
     */
    void
    setResources(UnitsMonitor *unitsMonitor, BuildingsMonitor *buildingsMonitor,
                 MapMonitor *mapMonitor, ServerMessenger *messenger,
                 const std::string &owner);

    void setMapData();

    // Functions to change the window's side panel
    bool change_view_to_unit();

    bool change_view_to_building();

    bool change_view_to_unit_group();

    bool on_button_release_event(GdkEventButton *event);

    void factory_next();

    void factory_change_unit(std::string &path);

    void factory_create_unit();

    void update_selection(int id);

    void process_selected_unit_action();

    void update_name(const std::string& name);
    void update_players(const std::vector<std::string>& players);
};

#endif //Z_TPGRUPAL_GAMEWINDOW_H
