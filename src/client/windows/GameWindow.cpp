#include <iostream>
#include <giomm.h>
#include <gdkmm.h>
#include "GameWindow.h"

#define SCREENWIDTH 1200
#define SCREENHEIGHT 800
#define FRAMERATE 10        //fps

GameWindow::GameWindow(BaseObjectType *cobject,
                       const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::ApplicationWindow(cobject)
{
    builder->get_widget_derived("GameArea", gameArea);

    gameArea->set_size_request(SCREENWIDTH * 6 / 7, SCREENHEIGHT);
    builder->get_widget("SidePanel", panel);
    builder->get_widget_derived("BuildingView", building_panel);
    builder->get_widget("UnitView", unit_panel);
    builder->get_widget("GroupView", group_panel);
    builder->get_widget("PanelDisplayLabel", panelLabel);
    // Logic for redrawing the map every frame
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

void GameWindow::setResources(PlayersMonitor *playersMonitor,
                              BuildingsMonitor *buildingsMonitor,
                              MapMonitor *mapMonitor,
                              ServerMessenger *messenger) {
    this->playersMonitor = playersMonitor;
    this->buildingsMonitor = buildingsMonitor;
    this->mapMonitor = mapMonitor;
    this->messenger = messenger;
    gameArea->setResources(playersMonitor, buildingsMonitor, mapMonitor);
}


bool GameWindow::change_view_to_unit() {
    for (auto child : panel->get_children()) {
        panel->remove(*child);
    }

    panel->pack_start(*unit_panel);
    return true;
}

bool GameWindow::change_view_to_building() {
    for (auto child : panel->get_children()) {
        panel->remove(*child);
    }
    panel->pack_start(*building_panel);

    panelLabel->set_text(building_panel->get_label());
    return true;
}

bool GameWindow::change_view_to_unit_group() {
    for (auto child : panel->get_children()) {
        panel->remove(*child);
    }

    panel->pack_start(*group_panel);
    return false;
}
