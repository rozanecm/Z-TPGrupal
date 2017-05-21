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
    refBuilder->get_widget_derived("GameWindow", window);
}


GameBuilder::~GameBuilder() {
    if (window) {
        delete window;
    }
}

GameWindow *GameBuilder::get_window() {
    return window;
}



