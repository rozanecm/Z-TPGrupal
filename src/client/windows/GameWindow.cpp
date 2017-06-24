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


    building_panel->next_button()->
            signal_clicked().connect(
            sigc::mem_fun(*this, &GameWindow::factory_next));

    building_panel->create_button()->
            signal_clicked().connect(
            sigc::mem_fun(*this, &GameWindow::factory_create_unit));


    building_panel->prev_button()->
            signal_clicked().connect(
            sigc::mem_fun(*this, &GameWindow::factory_prev));
    // Logic for redrawing the map every frame
    sigc::slot<bool> mySlot = sigc::mem_fun(*this, &GameWindow::onTimeout);
    Glib::signal_timeout().connect(mySlot, 1000 / FRAMERATE);

    show_all_children();
    add_events(Gdk::EventMask::KEY_PRESS_MASK);
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
    update_side_panels();
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
    unit_panel->update_portrait(selected_unit.getType());
    update_side_panels();
    return true;
}

bool GameWindow::change_view_to_building() {
    for (auto child : panel->get_children()) {
        panel->remove(*child);
    }
    panel->pack_start(*building_panel);
    panelLabel->set_text(building_panel->get_label());
    update_side_panels();

    return true;
}

bool GameWindow::change_view_to_unit_group() {
    for (auto child : panel->get_children()) {
        panel->remove(*child);
    }

    panel->pack_start(*group_panel);
    update_side_panels();

    return true;
}


bool GameWindow::on_button_release_event(GdkEventButton *event) {
    if (event->button == GDK_BUTTON_SECONDARY) {
        if (!(selected_unit.get_owner() == me)) {
            return true;
        }

        if (gameArea->unit_selected() || gameArea->buildings_selected()) {
            // We already are selecting an unit, process attack
            process_attack();
        } else {  // Click on empty place, movement
            process_movement();
        }
    } else if (event->button == GDK_BUTTON_PRIMARY) {
        if (gameArea->buildings_selected()) { // New building selected
            selected_building = buildingsMonitor->get_selected().at(0);

            change_view_to_building();
            messenger->send(
                    "factory-" + std::to_string(selected_building.get_ID())
                    + "-current");

            // Change selection status
            unit_selection = false;
            building_selection = true;
        } else if (gameArea->unit_selected()) { // New unit selected
            selected_unit = unitsMonitor->getSelectedUnits().at(0);
            change_view_to_unit();

            // Change selection status
            building_selection = false;
            unit_selection = true;
        }
    }
    return true;
}


void GameWindow::process_movement() const {
    int id = selected_unit.get_ID();
    std::pair<int, int> coords = gameArea->get_coords();
    std::cout << gameArea->get_coords().first << ", " <<
              gameArea->get_coords().second << std::endl;

    std::stringstream s;
    s << "mv-" << id << "-" << coords.first << "-" << coords.second;
    messenger->send(s.str());
}

void GameWindow::factory_next() {
    int id = selected_building.get_ID();
    messenger->send("factory-"+std::to_string(id)+"-next");
}

void GameWindow::update_selection(int id) {
    selection_id = id;
}

void GameWindow::factory_change_unit(std::string &path) {
    building_panel->change_unit(path);
}

void GameWindow::factory_create_unit() {
    int id = selected_building.get_ID();
    messenger->send("factory-"+std::to_string(id)+"-create");
}

void GameWindow::setMapData() {
    gameArea->setMapData();
}

void GameWindow::process_attack() {
    std::vector<Unit> units = unitsMonitor->getSelectedUnits();
    std::string target;
    if (units.size()) { // other unit selected
        Unit other = units.at(0);
        if (selected_unit.getTeam() == other.getTeam()) {
            return;
        }
        target = std::to_string(other.get_ID());
    }
    std::vector<Building> buildings = buildingsMonitor->get_selected();
    if (buildings.size()) {
        Building other = buildings.at(0);
        if (selected_unit.getTeam() == other.getTeam()) {
            return;
        }
        target = std::to_string(other.get_ID());
    }

    std::string attack = "atk-" + std::to_string(selected_unit.get_ID()) +
                         "-" + target;
    messenger->send(attack);
}

void GameWindow::update_name(const std::string &name) {
    me = name;
}

void GameWindow::update_players(const std::vector<std::string> &players) {
    this->players = players;
}

bool GameWindow::on_key_press_event(GdkEventKey *event) {
    // Clear selection
    if (event->keyval == GDK_KEY_Escape) {
        remove_side_panel();
    }
    return Gtk::Window::on_key_press_event(event);
}

void GameWindow::remove_side_panel() {
    selected_building = Building();
    selected_unit = Unit();
    for (auto child : panel->get_children()) {
            panel->remove(*child);
        }
    panelLabel->set_text("Z");
}

void GameWindow::update_side_panels() {
    int unit_id = selected_unit.get_ID();
    int building_id = selected_building.get_ID();
    if (!unit_id && !building_id) {
        remove_side_panel();
    } else if (unit_id) {
        // UpdateUnit the unit reference
        selected_unit = unitsMonitor->get_unit(unit_id);
        unit_panel->set_name(selected_unit.get_unit_name());
        unit_panel->set_owner(selected_unit.get_owner());
        unit_panel->set_max_hp(selected_unit.get_max_hp());
        unit_panel->set_hp(selected_unit.get_hp());
    } else {
        selected_building = buildingsMonitor->get_building(building_id);
        building_panel->set_max_hp(selected_building.get_max_hp());
        building_panel->set_hp(selected_building.get_hp());
        building_panel->set_owner(selected_building.get_owner());
        building_panel->set_time_left(selected_building.get_time_left());
    }
}

void GameWindow::factory_prev() {
    int id = selected_building.get_ID();
    messenger->send("factory-"+std::to_string(id)+"-prev");
}

void GameWindow::update_factory_panel(const std::string &type, int fire_rate,
                                      int hp) {
    building_panel->set_unit_hp(hp);
    building_panel->set_unit_fire_rate(fire_rate);
    building_panel->set_unit_type(type);
}

void GameWindow::update_factory_timer(int minutes, int seconds) {
    building_panel->set_time_left(std::pair<int, int>(minutes, seconds));
}
