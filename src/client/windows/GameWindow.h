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
    GameArea* gameArea;
    Gtk::Box* panel;
    UnitPanel* unit_panel;
    BuildingPanel* building_panel;
    Gtk::Box* group_panel;
    Gtk::Label* panelLabel;

    UnitsMonitor *unitsMonitor;
    BuildingsMonitor *buildingsMonitor;
    MapMonitor *mapMonitor;
    ServerMessenger *messenger;

    int selection_id = 0;
protected:
    bool onTimeout();

public:
    GameWindow(BaseObjectType* cobject,
               const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~GameWindow();

    /**
     * Saves resources' monitors and passes them to the gameArea.
     */
    void
    setResources(UnitsMonitor *unitsMonitor,
                 BuildingsMonitor *buildingsMonitor,
                 MapMonitor *mapMonitor, ServerMessenger *messenger);
    // Functions to change the window's side panel
    bool change_view_to_unit();
    bool change_view_to_building();
    bool change_view_to_unit_group();

    void factory_next();
    void factory_change_unit(std::string& path);

    void factory_create_unit();
    void update_selection(int id);
};

#endif //Z_TPGRUPAL_GAMEWINDOW_H
