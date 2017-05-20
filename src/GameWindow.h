#ifndef Z_TPGRUPAL_GAMEWINDOW_H
#define Z_TPGRUPAL_GAMEWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/builder.h>
#include <gtkmm/box.h>
#include "GameArea.h"

class GameWindow : public Gtk::Window {
    GameArea* gameArea;

public:
    GameWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~GameWindow();

protected:
    Gtk::Box mainGrid;

    bool onTimeout();

};

#endif //Z_TPGRUPAL_GAMEWINDOW_H
