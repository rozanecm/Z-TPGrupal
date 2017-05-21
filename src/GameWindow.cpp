#include <iostream>
#include <giomm.h>
#include <gdkmm.h>
#include "GameWindow.h"

#define SCREENWIDTH 1200
#define SCREENHEIGHT 800
#define FRAMERATE 10        //fps

GameWindow::GameWindow(BaseObjectType *cobject,
                       const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::Window(cobject)
{
    builder->get_widget_derived("GameArea", gameArea);

    gameArea->set_size_request(SCREENWIDTH * 6 / 7, SCREENHEIGHT);

    sigc::slot<bool> mySlot = sigc::mem_fun(*this, &GameWindow::onTimeout);
    Glib::signal_timeout().connect(mySlot, 1000/FRAMERATE);
    show_all_children();

}

GameWindow::~GameWindow() {
}

bool GameWindow::onTimeout() {
    // force our program to redraw the entire thing
    auto win = get_window();
    if (win) {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                         get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
    return true;
}


