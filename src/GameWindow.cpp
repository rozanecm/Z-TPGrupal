#include <iostream>
#include "GameWindow.h"


GameWindow::GameWindow() {
    //Load the GtkBuilder file and instantiate its widgets:
    auto refBuilder = Gtk::Builder::create();
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
    refBuilder->get_widget("GameWindow", window);
    refBuilder->get_widget("Portrait", portrait);

    update_portrait("assets/portraits/sample.png");
}

void GameWindow::update_portrait(const std::string& img_path) const {
    if (portrait) {
        portrait->set(img_path);
    }
}

GameWindow::~GameWindow() {
    if (window) {
        delete window;
    }
}

Gtk::Window *GameWindow::get_window() {
    return window;
}

