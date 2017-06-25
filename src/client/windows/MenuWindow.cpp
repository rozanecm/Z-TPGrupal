#include <gtkmm/label.h>
#include "MenuWindow.h"

MenuWindow::MenuWindow(BaseObjectType *cobject,
                       const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject)
{

    builder->get_widget("AvailableLobbies", available_lobbies);
    builder->get_widget("CreateLobbyButton", create_button);
    builder->get_widget("JoinLobbyButton", join_button);
    builder->get_widget("LobbyEntry", lobby_entry);
    join_button->signal_clicked().connect(
            sigc::mem_fun(*this,
                          &MenuWindow::join_click));

    create_button->signal_clicked().connect(
            sigc::mem_fun(*this,
                         &MenuWindow::create_click));
}

void MenuWindow::join_click() {
    std::string lobby = lobby_entry->get_text();
    try {
        std::stoi(lobby);
    } catch(std::invalid_argument& e) {
        std::cout << "Invalid lobby ID. Insert only numbers!" << std::endl;
        return;
    }
    messenger->send("joinlobby-" + lobby);
}

void MenuWindow::load_messenger(ServerMessenger *messenger) {
    this->messenger = messenger;
}

void MenuWindow::create_click() {
    messenger->send("createlobby");
}

void MenuWindow::update_lobbies(const std::vector<std::string> &lobbies) {
    std::stringstream text;
    for (const std::string& lobby : lobbies) {
        text << lobby << std::endl;
    }
    available_lobbies->set_text(text.str());
}

bool MenuWindow::joined_lobby() {
    return joined_successfully;
}

void MenuWindow::on_show() {
    joined_successfully = false;
    Gtk::Widget::on_show();
}

void MenuWindow::join_lobby() {
    joined_successfully = true;
    hide();
}
