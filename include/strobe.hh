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

    float x_, y_;

public:

    Point (float x, float y) : x_(x), y_(y) {}
    Point () : x_(0.0), y_(0.0) {}

    inline float GetX () const { return x_; }
    inline float GetY () const { return y_; }

    inline void Set (float x, float y) { 
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

    float radius_, theta_, cx_, cy_;

public:

    PolarPoint (float radius, float theta) : 
        radius_(radius), theta_(theta), cx_(WIDTH / 2.0), cy_(HEIGHT / 2.0) {}

    inline float GetRadius () const { return radius_; }
    inline float GetTheta () const { return theta_; }

    inline void Rotate (float theta) { theta_ += theta; }

    Point GetXY (float theta) const;
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

    inline void Draw (float theta) const {
        Draw (theta, col_);
    }

    inline const PolarPoint& GetStart () const { return p1_; } 
    inline const PolarPoint& GetEnd () const { return p2_; } 

    inline void Rotate (float theta) {  
        p1_.Rotate (theta);
        p2_.Rotate (theta);
    }

    void Draw (float theta, ALLEGRO_COLOR col) const;

    Segment SaveAt (float theta) const;
};

/*
 * Line segment that will fade a bit every time it is drawn
 */
class FadeSegment {
    
    PolarPoint p1_, p2_;
    ALLEGRO_COLOR col_;
    float opacity_, step_;
    bool gone_;

public:

    FadeSegment (const Segment &seg) : p1_(seg.GetStart ()),
        p2_(seg.GetEnd ()), col_(mkcol (255, 255, 255, 255)),
        opacity_(255.0), step_(THETA_DELTA * 0.67), gone_(false) {}

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

    Rectangle (float x1, float y1, float x2, float y2) : 
        p1_(x1,y1), p2_(x2,y2), col_(mkcol(255,255,255,30)) {}

    Rectangle (const Point &p1, const Point &p2) : 
        p1_(p1), p2_(p2), col_(mkcol(255,255,255,30)) {}

    inline void Draw () const {
        Draw (col_); 
    }

    void Draw (ALLEGRO_COLOR col) const;
    bool Contains (const Segment &seg, float theta) const;

};

/*
 * Reference point upon which all other segments base their position
 */
class Reference {

    float theta_;
    bool looped_;
    Segment tick_; /* for visual reference only */
    ALLEGRO_COLOR col_;

public:

    Reference () : theta_(0.0), looped_(false),
        tick_(Segment(
                    PolarPoint (WIDTH/2.0 - 15, 0), 
                    PolarPoint (WIDTH/2.0, 0))),
        col_(mkcol (30, 255, 10, 255)) {}

    inline float Theta () const { return theta_; }
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

    float cx_, cy_, radius_;
    ALLEGRO_COLOR col_;

public:

    Border (float radius) : cx_(WIDTH / 2.0), cy_(HEIGHT / 2.0), 
            radius_(radius), col_(mkcol(255, 0, 0, 255)) {}

    Border () : cx_(WIDTH / 2.0), cy_(HEIGHT / 2.0),
            radius_(WIDTH / 2.0), col_(mkcol(255, 0, 0, 255)) {}

    inline void Draw () const { 
        Draw (col_);
    }

    void Draw (ALLEGRO_COLOR col) const;
};

struct GameState {
    bool running, paused, debug;
    Border border;
    Reference ref_mark;
    std::vector< Segment > segments;
    std::vector< FadeSegment > fade;
    GameState () : running(true), paused(false), debug(false) {}
};

#endif /*STROBE_HH__*/
