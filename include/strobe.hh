#ifndef STROBE_HH__
#define STROBE_HH__

#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define WIDTH 600
#define HEIGHT 600
#define FPS 60.0
#define ANGLE_MAX 360.0
#define ANGLE_STEPS 60.0
#define ANGLE_DELTA (ANGLE_MAX / ANGLE_STEPS)

#define RADIANS(deg) ((deg) * 0.0174533)

ALLEGRO_COLOR mkcol (int r, int g, int b, int a);

/* 
 * Basic cartesian point
 */
class Point {

    float x_, y_;

public:

    Point (float x, float y) : x_(x), y_(y) {}
    inline float GetX () const { return x_; }
    inline float GetY () const { return y_; }
};

/*
 * Points are represented by a distance from the origin and
 * a rotation from the x-axis
 */
class RadialPoint {

    float radius_, angle_, cx_, cy_;

public:

    RadialPoint (float radius, float angle) : 
        radius_(radius), angle_(angle), cx_(WIDTH / 2.0), cy_(HEIGHT / 2.0) {}
    Point GetXY (float angle) const;
};

/*
 * Line segment
 */
class Segment {

    RadialPoint p1_, p2_;
    ALLEGRO_COLOR col_;
    
public:

    Segment (const RadialPoint &p1, const RadialPoint &p2) : 
        p1_(p1), p2_(p2), col_(mkcol (0, 0, 255, 255)) {}

    inline void Draw (float angle) const {
        Draw (angle, col_);
    }

    inline const RadialPoint& GetStart () const { return p1_; } 
    inline const RadialPoint& GetEnd () const { return p2_; } 

    void Draw (float angle, ALLEGRO_COLOR col) const;
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
        p1_(x1,y1), p2_(x2,y2), col_(mkcol(30,255,10,30)) {}

    Rectangle (const Point &p1, const Point &p2) : 
        p1_(p1), p2_(p2), col_(mkcol(30,255,10,30)) {}

    inline void Draw () const {
        Draw (col_); 
    }

    void Draw (ALLEGRO_COLOR col) const;
    bool Contains (const Segment &seg, float angle) const;

};

/*
 * Reference point upon which all other segments base their position
 */
class Reference {

    float angle_;
    bool looped_;
    Segment tick_; /* for visual reference only */
    ALLEGRO_COLOR col_;

public:

    Reference () : angle_(0.0), looped_(false),
        tick_(Segment(
                    RadialPoint (WIDTH/2.0 - 15, 0), 
                    RadialPoint (WIDTH/2.0, 0))),
        col_(mkcol (30, 255, 10, 255)) {}

    inline float Angle () const { return angle_; }
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
    bool running, paused;
    Border border;
    Reference ref_mark;
    std::vector< Segment > segments;
    GameState () : running(true), paused(false) {}
};

#endif /*STROBE_HH__*/
