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
    /* declare vector which contains imgs composing blue flag animation */
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> blueFlagVector;

    /* declare counter used to know which of the flag imgs
     * which compose the flag's animation should be showed */
    int flagCounter;

    /* DRAWING METHODS */
    void drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &ptr,
                           int xCoordinate, int yCoordinate);

    void displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &refPtr);
};


#endif //Z_TPGRUPAL_GAMEAREA_H
