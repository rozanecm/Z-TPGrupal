#ifndef Z_TPGRUPAL_GAMEAREA_H
#define Z_TPGRUPAL_GAMEAREA_H


#include <gtkmm/drawingarea.h>

class GameArea : public Gtk::DrawingArea{
public:
    GameArea();

    virtual ~GameArea();

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

private:
    Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file("res/portraits/sample.png");
};


#endif //Z_TPGRUPAL_GAMEAREA_H
