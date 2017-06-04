#ifndef Z_TPGRUPAL_GAMEAREA_H
#define Z_TPGRUPAL_GAMEAREA_H


#include <gtkmm/drawingarea.h>
#include <utility>
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "Camera.h"
#include "FlagEnum.h"
#include "TeamEnum.h"
#include "ActionsEnum.h"
#include "UnitsEnum.h"
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

    static void keyboardPressed();

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

private:
    /* shared resources */
    PlayersMonitor *playersMonitor;
    BuildingsMonitor *buildingsMonitor;
    MapMonitor *mapMonitor;

    Camera camera;

    /* general resources */
    std::map<std::string, Glib::RefPtr<Gdk::Pixbuf>> tiles;

    Glib::RefPtr<Gdk::Pixbuf> someImg;

    /* map holding all flags */
    std::map<FlagEnum, std::vector<Glib::RefPtr<Gdk::Pixbuf>>> flags;

    /* map holding all units */
    std::map<TeamEnum,
            std::map<UnitsEnum,
                    std::map<ActionsEnum,
                            std::vector<Glib::RefPtr<Gdk::Pixbuf>>>>>
            unitsAnimations;

    /* declare counter used to know which of the flag imgs
     * which compose the flag's animation should be showed */
    unsigned long flagCounter;

    /* DRAWING METHODS */
    void drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &ptr,
                           int xCoordinate, int yCoordinate);

    void displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &refPtr,
                              int xCoordinate, int yCoordinate);

    void drawBaseMap(const Cairo::RefPtr<Cairo::Context> &cr,
                     std::pair<unsigned int, unsigned int> cameraPosition);

    void drawTileAt(const Cairo::RefPtr<Cairo::Context> &cr,
                    unsigned int xCoordinate, unsigned int yCoordinate,
                    std::string terrainType);

    /* Event handling methods */
    bool on_key_press_event(GdkEventKey *event) override;

    bool on_button_press_event(GdkEventButton *event) override;

    bool on_button_release_event(GdkEventButton *event) override;

    /* vars. for event handling */
    gdouble xStartCoordinate;
    gdouble xFinishCoordinate;
    gdouble yStartCoordinate;
    gdouble yFinishCoordinate;
    bool selectionMade;

    void processSelection();

    void loadFlagAnimations();

    void loadUnitsResources();

    void loadGruntFireAnimations();

    void loadLaserFireAnimations();

    void loadPsychoFireAnimations();

    void loadPyroFireAnimations();

    void loadSniperFireAnimations();

    void loadToughFireAnimations();

    void loadBlueGruntFireAnimations();

    void loadGreenGruntFireAnimations();

    void loadRedGruntFireAnimations();

    void loadYellowGruntFireAnimations();

    void loadBlueLaserFireAnimations();
};


#endif //Z_TPGRUPAL_GAMEAREA_H
