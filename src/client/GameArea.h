#ifndef Z_TPGRUPAL_GAMEAREA_H
#define Z_TPGRUPAL_GAMEAREA_H


#include <gtkmm/drawingarea.h>
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "Camera.h"
#include <map>
#include <string>
#include <vector>

class GameArea : public Gtk::DrawingArea{
public:
    virtual ~GameArea();
    GameArea(BaseObjectType* cobject,
             const Glib::RefPtr<Gtk::Builder>& builder);

    /**
     * initialize shared resources.
     */
    void
    setResources(PlayersMonitor *playersMonitor,
                 BuildingsMonitor *buildingsMonitor,
                 MapMonitor *mapMonitor);

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

private:
    PlayersMonitor *playersMonitor;
    BuildingsMonitor *buildingsMonitor;
    MapMonitor *mapMonitor;
    Camera camera;

    Glib::RefPtr<Gdk::Pixbuf> someImg;

    /* declare vector which contains imgs composing blue flag animation */
    std::vector<Glib::RefPtr<Gdk::Pixbuf>> blueFlagVector;

    /* declare counter used to know which of the flag imgs
     * which compose the flag's animation should be showed */
    unsigned long flagCounter;

    /* DRAWING METHODS */
    void drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &ptr,
                           int xCoordinate, int yCoordinate);

    void displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &refPtr,
                              int xCoordinate, int yCoordinate);

    void drawBaseMap(const Cairo::RefPtr<Cairo::Context> &cr);

    void drawTileAt(const Cairo::RefPtr<Cairo::Context> &cr,
                    unsigned int xCoordinate, unsigned int yCoordinate,
                    std::string terrainType);

    std::map<std::string, Glib::RefPtr<Gdk::Pixbuf>> tiles;
};


#endif //Z_TPGRUPAL_GAMEAREA_H
