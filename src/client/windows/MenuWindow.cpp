#include "MenuWindow.h"

MenuWindow::MenuWindow(BaseObjectType *cobject,
                       const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject)
{

    builder->get_widget("AvailableLobbies", available_lobbies);
    builder->get_widget("CreateLobbyButton", create_button);
    builder->get_widget("JoinLobbyButton", join_button);

    join_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                  &MenuWindow::join_lobby));
}

void MenuWindow::join_lobby() {
}
