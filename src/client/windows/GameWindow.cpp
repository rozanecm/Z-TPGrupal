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
    Glib::signal_timeout().connect(mySlot, 1000 / FRAMERATE);

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
                              ServerMessenger *messenger,
                              const std::string &owner) {
    this->unitsMonitor = unitsMonitor;
    this->buildingsMonitor = buildingsMonitor;
    this->mapMonitor = mapMonitor;
    this->messenger = messenger;
    gameArea->setResources(unitsMonitor, buildingsMonitor, mapMonitor, owner);
}


bool GameWindow::change_view_to_unit() {
    for (auto child : panel->get_children()) {
        panel->remove(*child);
    }

    panel->pack_start(*unit_panel);
    panelLabel->set_text(unit_panel->get_label());

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
    if (gameArea->buildings_selected()) { // Change view to building
        selected_building = buildingsMonitor->get_selected().at(0);

        // Change selection status
        unit_selection = false;
        building_selection = true;
        change_view_to_building();
        return true;
    }

    if (gameArea->unit_selected()) { // New unit selected
        if (unit_selection) { // We already are selecting an unit, process
                                // attack
            process_selected_unit_action();
        } else { // Select unit
            selected_unit = unitsMonitor->getSelectedUnits().at(0);

            std::string name = "grunt";
            if (portraits.find(selected_unit.getType()) != portraits.end()) {
                name = portraits.find(selected_unit.getType())->second;
            }
            unit_panel->set_name(name);
            std::string path = "res/portraits/" + name + ".png";
            portrait->set(path);
            change_view_to_unit();
        }
        // Change selection status
        building_selection = false;
        unit_selection = true;
        return true;
    }

    // Click on empty place
    if (unit_selection) { // Movement
        int id = selected_unit.get_ID();
        std::pair<int, int> coords = gameArea->get_coords();
        std::cout << gameArea->get_coords().first << ", " <<
                  gameArea->get_coords().second << std::endl;

        std::stringstream s;
        s << "mv-" << id << "-" << coords.first << "-" << coords.second;
        messenger->send(s.str());
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

void GameWindow::setMapData() {
    gameArea->setMapData();
}

void GameWindow::process_selected_unit_action() {
    std::vector<Unit> units = unitsMonitor->getSelectedUnits();
    if (units.size()) { // other unit selected
        if (unit_selection) {
            Unit other = units.at(0);
            if (selected_unit.getTeam() != other.getTeam()) {
                // attack
            }
            return;
        }
    }
}
