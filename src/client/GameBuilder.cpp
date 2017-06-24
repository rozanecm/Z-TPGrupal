#include <iostream>
#include "GameBuilder.h"
#include "windows/ResultWindow.h"


GameBuilder::GameBuilder() :
    init_window(nullptr),
    menu_window(nullptr),
    lobby_window(nullptr),
    window(nullptr),
    result_window(nullptr)
{
    //Load the GtkBuilder file and instantiate its widgets:
    start();

}

void GameBuilder::start() {
    clean();
    this->refBuilder = Gtk::Builder::create();
    try {
        this->refBuilder->add_from_file("Z.glade");
    }
    catch (const Glib::FileError &ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }
    catch (const Glib::MarkupError &ex) {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return;
    }
    catch (const Gtk::BuilderError &ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return;
    }

    // Save the widget refs in the class attributes
    this->refBuilder->get_widget_derived("GameWindow", this->window);
    this->refBuilder->get_widget_derived("InitialWindow", this->init_window);
    this->refBuilder->get_widget_derived("LobbyWindow", this->lobby_window);
    this->refBuilder->get_widget_derived("MenuWindow", this->menu_window);
    this->refBuilder->get_widget_derived("ResultWindow", this->result_window);
}


GameBuilder::~GameBuilder() {
    clean();
}

GameWindow *GameBuilder::get_window() {
    return window;
}

InitialWindow *GameBuilder::get_initial_window() {
    return init_window;
}

LobbyWindow *GameBuilder::get_lobby_window() {
    return lobby_window;
}

MenuWindow *GameBuilder::get_menu_window() {
    return menu_window;
}

ResultWindow *GameBuilder::get_result_window() {
    return result_window;
}

void GameBuilder::clean() {
    if (window) {
        delete window;
    }
    if (init_window) {
        delete init_window;
    }
    if (lobby_window) {
        delete lobby_window;
    }
    if (menu_window) {
        delete menu_window;
    }
    if (result_window) {
        delete result_window;
    }
}



