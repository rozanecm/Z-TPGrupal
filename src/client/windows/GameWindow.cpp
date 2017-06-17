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
    builder->get_widget_derived("UnitView", unit_panel);
    builder->get_widget("GroupView", group_panel);
    builder->get_widget("PanelDisplayLabel", panelLabel);
    builder->get_widget("Portrait", portrait);


    building_panel->next_button()->
            signal_clicked().connect(
            sigc::mem_fun(*this, &GameWindow::factory_next));

    building_panel->create_button()->
            signal_clicked().connect(
            sigc::mem_fun(*this, &GameWindow::factory_create_unit));

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

void GameWindow::setResources(UnitsMonitor *unitsMonitor,
                              BuildingsMonitor *buildingsMonitor,
                              MapMonitor *mapMonitor,
                              ServerMessenger *messenger) {
    this->unitsMonitor = unitsMonitor;
    this->buildingsMonitor = buildingsMonitor;
    this->mapMonitor = mapMonitor;
    this->messenger = messenger;
    gameArea->setResources(unitsMonitor, buildingsMonitor, mapMonitor);
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


const std::map<UnitsEnum, std::string> portraits = {
        {UnitsEnum::GRUNT, std::string("grunt")}
};

bool GameWindow::on_button_release_event(GdkEventButton *event) {
    std::vector<Unit> units = unitsMonitor->getSelectedUnits();
    if (units.size()) {
        change_view_to_unit();
        Unit u = units.at(0);
        std::string name = "grunt";
        if (portraits.find(u.getType()) != portraits.end()) {
            name = portraits.find(u.getType())->second;

        }
        std::string path = "res/portraits/" + name + ".png";
        portrait->set(path);
    }
    return true;
}

void GameWindow::factory_next() {
    messenger->send("nextunit-" + selection_id);
    std::string path = "res/portraits/psycho.png";
    factory_change_unit(path);
}

void GameWindow::update_selection(int id) {
    selection_id = id;
}

void GameWindow::factory_change_unit(std::string &path) {
    building_panel->change_unit(path);
}

void GameWindow::factory_create_unit() {
    messenger->send("createunit-" + selection_id);
}
