#ifndef STROBE_HH__
#define STROBE_HH__

#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define WIDTH 600
#define HEIGHT 600
#define FPS 60.0
#define THETA_MAX 360.0
#define THETA_STEPS 60.0
#define THETA_DELTA (THETA_MAX / THETA_STEPS)

#define RADIANS(deg) ((deg) * 0.0174533)
#define DEGREES(rad) ((rad) * 57.2958)

ALLEGRO_COLOR mkcol (int r, int g, int b, int a);

class PolarPoint;

/* 
 * Basic cartesian point
 */
class Point {

    double x_, y_;

public:

    Point (double x, double y) : x_(x), y_(y) {}
    Point () : x_(0.0), y_(0.0) {}

    inline double GetX () const { return x_; }
    inline double GetY () const { return y_; }

    inline void Set (double x, double y) { 
        x_ = x; 
        y_ = y;
    }

    PolarPoint GetPolar () const;
};

/*
 * Points are represented by a distance from the origin and
 * a rotation from the x-axis
 */
class PolarPoint {

    double radius_, theta_, cx_, cy_;

public:

    PolarPoint (double radius, double theta) : 
        radius_(radius), theta_(theta), cx_(WIDTH / 2.0), cy_(HEIGHT / 2.0) {}

    inline double GetRadius () const { return radius_; }
    inline double GetTheta () const { return theta_; }

    inline void Rotate (double theta) { theta_ += theta; }

    Point GetXY (double theta) const;
};

/*
 * Line segment
 */
class Segment {

    PolarPoint p1_, p2_;
    ALLEGRO_COLOR col_;
    
public:

    Segment (const PolarPoint &p1, const PolarPoint &p2) : 
        p1_(p1), p2_(p2), col_(mkcol (0, 0, 255, 255)) {}

    inline void Draw (double theta) const {
        Draw (theta, col_);
    }

    inline const PolarPoint& GetStart () const { return p1_; } 
    inline const PolarPoint& GetEnd () const { return p2_; } 

    inline void Rotate (double theta) {  
        p1_.Rotate (theta);
        p2_.Rotate (theta);
    }

    void Draw (double theta, ALLEGRO_COLOR col) const;

    Segment SaveAt (double theta) const;
};

/*
 * Line segment that will fade a bit every time it is drawn
 */
class FadeSegment {
    
    PolarPoint p1_, p2_;
    ALLEGRO_COLOR col_;
    double opacity_, step_;
    bool gone_;

public:

    FadeSegment (const Segment &seg) : p1_(seg.GetStart ()),
        p2_(seg.GetEnd ()), col_(mkcol (255, 255, 255, 255)),
        opacity_(255.0), step_(THETA_DELTA * 0.75), gone_(false) {}

    inline bool Remove () const { return gone_; }

    void Draw ();

};

/*
 * Basic rectangel overlayed on a fixed portion of the display.
 * This is *not* represented in polar coordinates and does *not* rotate
 * about the centerpoint as segments to
 */
class Rectangle {

    /* Bottom-left and top-right coordinates */
    Point p1_, p2_;
    ALLEGRO_COLOR col_;

public:

    Rectangle (double x1, double y1, double x2, double y2) : 
        p1_(x1,y1), p2_(x2,y2), col_(mkcol(255,255,255,30)) {}

    Rectangle (const Point &p1, const Point &p2) : 
        p1_(p1), p2_(p2), col_(mkcol(255,255,255,30)) {}

    inline void Draw () const {
        Draw (col_); 
    }

    void Draw (ALLEGRO_COLOR col) const;
    bool Contains (const Segment &seg, double theta) const;

};

/*
 * Reference point upon which all other segments base their position
 */
class Reference {

    double theta_;
    bool looped_;
    Segment tick_; /* for visual reference only */
    ALLEGRO_COLOR col_;

public:

    Reference () : theta_(0.0), looped_(false),
        tick_(Segment(
                    PolarPoint (WIDTH/2.0 - 15, 0), 
                    PolarPoint (WIDTH/2.0, 0))),
        col_(mkcol (30, 255, 10, 255)) {}

    inline double Theta () const { return theta_; }
    inline bool Looped () const { return looped_; }

    inline void Draw () const {
        Draw (col_);
    }

    void Draw (ALLEGRO_COLOR col) const; 
    void Tick ();

};

/*
 * Simple wrapper class for the border of the printable region
 * Currently just a circle - would need work to allow other shapes
 */
class Border {

    double cx_, cy_, radius_;
    ALLEGRO_COLOR col_;

public:

    Border (double radius) : cx_(WIDTH / 2.0), cy_(HEIGHT / 2.0), 
            radius_(radius), col_(mkcol(255, 0, 0, 255)) {}

    Border () : cx_(WIDTH / 2.0), cy_(HEIGHT / 2.0),
            radius_(WIDTH / 2.0), col_(mkcol(255, 0, 0, 255)) {}

    inline void Draw () const { 
        Draw (col_);
    }

    void Draw (ALLEGRO_COLOR col) const;
};

struct GameState {
    bool running, paused, debug, step;
    Border border;
    Reference ref_mark;
    std::vector< Segment > segments;
    std::vector< FadeSegment > fade;
    GameState () : running(true), paused(false), debug(false), step(false) {}
};

#endif /*STROBE_HH__*/
