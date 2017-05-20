#include <iostream>
#include "GameBuilder.h"


GameBuilder::GameBuilder() {
    //Load the GtkBuilder file and instantiate its widgets:
    refBuilder = Gtk::Builder::create();
    try
    {
        refBuilder->add_from_file("src/Z.glade");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }
    catch(const Glib::MarkupError& ex)
    {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return;
    }
    catch(const Gtk::BuilderError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return;
    }

    // Save the widget refs in the class attributes
    refBuilder->get_widget("GameBuilder", window);
    refBuilder->get_widget("Portrait", portrait);
    refBuilder->get_widget("SidePanel", panel);
    refBuilder->get_widget("BuildingView", building_panel);
    refBuilder->get_widget("UnitView", unit_panel);
    refBuilder->get_widget("GroupView", group_panel);
    refBuilder->get_widget("Create", button);
    update_portrait("assets/portraits/sample.png");
}

void GameBuilder::update_portrait(const std::string& img_path) const {
    if (portrait) {
        portrait->set(img_path);
    }
}

GameBuilder::~GameBuilder() {
    if (window) {
        delete window;
    }
}

Gtk::Window *GameBuilder::get_window() {
    return window;
}

bool GameBuilder::change_view_to_unit() {
    panel->pack_start(*unit_panel);
    return true;
}

bool GameBuilder::change_view_to_building() {
    panel->pack_start(*building_panel);
    return true;
}

bool GameBuilder::change_view_to_unit_group() {
    panel->pack_start(*group_panel);
    return false;
}


