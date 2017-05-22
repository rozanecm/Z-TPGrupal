#ifndef Z_TPGRUPAL_GAMEWINDOW_H
#define Z_TPGRUPAL_GAMEWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/builder.h>
#include <gtkmm/box.h>
#include "GameArea.h"
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"

class GameWindow : public Gtk::Window {
    GameArea* gameArea;

public:
    GameWindow(BaseObjectType* cobject,
               const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~GameWindow();

    /**
     * Saves resources' monitors and passes them to the gameArea.
     */
    void
    setResources(PlayersMonitor *playersMonitor,
                 BuildingsMonitor *buildingsMonitor,
                 MapMonitor *mapMonitor);

protected:
    Gtk::Box mainGrid;

    bool onTimeout();

private:
    PlayersMonitor *playersMonitor;
    BuildingsMonitor *buildingsMonitor;
    MapMonitor *mapMonitor;
};

#endif //Z_TPGRUPAL_GAMEWINDOW_H
