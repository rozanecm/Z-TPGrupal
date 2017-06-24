#include <iostream>
#include "GameBuilder.h"


GameBuilder::GameBuilder() {
    //Load the GtkBuilder file and instantiate its widgets:
    refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_file("Z.glade");
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
    refBuilder->get_widget_derived("GameWindow", window);
    refBuilder->get_widget_derived("InitialWindow", init_window);
    refBuilder->get_widget_derived("LobbyWindow", lobby_window);
    refBuilder->get_widget_derived("MenuWindow", menu_window);
}


GameBuilder::~GameBuilder() {
    if (window) {
        delete window;
    }
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



