#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "strobe.hh"

PolarPoint Point::GetPolar () const {
    double radius = sqrt (x_ * x_ + y_ * y_);
    double theta = DEGREES(atan2 (y_, x_));
    return PolarPoint (radius, theta);
}

Point PolarPoint::GetXY (double theta) const {
    double x = cx_ + radius_ * cos (RADIANS(theta_ + theta));
    double y = cy_ + radius_ * sin (RADIANS(theta_ + theta));
    return Point(x,y);
}

void Segment::Draw (double theta, ALLEGRO_COLOR col) const {
    Point p1 = p1_.GetXY (theta);
    Point p2 = p2_.GetXY (theta);
    al_draw_line (p1.GetX (), p1.GetY (), p2.GetX (), p2.GetY (), col, 1);
}

Segment Segment::SaveAt (double theta) const {
    PolarPoint a = GetStart (), b = GetEnd ();
    return Segment (PolarPoint (a.GetRadius (), a.GetTheta () + theta),
            PolarPoint (b.GetRadius (), b.GetTheta () + theta));
}

void FadeSegment::Draw () {
    Point p1 = p1_.GetXY (0);
    Point p2 = p2_.GetXY (0);
    al_draw_line (p1.GetX (), p1.GetY (), p2.GetX (), p2.GetY (), col_, 3);
    opacity_ -= step_;
    if (opacity_ <= 0.0) {
        gone_ = true;
    }
    col_ = mkcol (255, 255, 255, opacity_);
}

void Rectangle::Draw (ALLEGRO_COLOR col) const {
    al_draw_filled_rectangle (p1_.GetX (), p1_.GetY(),
            p2_.GetX (), p2_.GetY (), col);
}

bool Rectangle::Contains (const Segment &seg, double theta) const {
    Point start = seg.GetStart ().GetXY (theta);
    Point end = seg.GetEnd ().GetXY (theta);
    bool start_in = false, end_in = false;
    double x = start.GetX (), y = start.GetY ();

    /*
     * FIXME: This comparison only works for areas that reside below
     * the y = HEIGHT / 2.0 horizon.
     * Adjust to work for every quadrant
     */

    start_in = ((x > p1_.GetX () && x < p2_.GetX ()) && 
            (y > p2_.GetY () && y < p1_.GetY ()));
    x = end.GetX ();
    y = end.GetY ();
    end_in = ((x > p1_.GetX () && x < p2_.GetX ()) && 
            (y > p2_.GetY () && y < p1_.GetY ()));
    return (start_in && end_in);
}


void Reference::Draw (ALLEGRO_COLOR col) const {
    tick_.Draw (Theta (), col);
}

void Reference::Tick () {
    theta_ += THETA_DELTA;
    if (theta_ >= THETA_MAX) {
        theta_ -= THETA_MAX;
        looped_ = true;
    } else {
        looped_ = false;
    }
}

void Border::Draw (ALLEGRO_COLOR col) const {
    al_draw_circle (cx_, cy_, radius_, col, 1.0);
}

ALLEGRO_COLOR mkcol (int r, int g, int b, int a) {
    float fr = (float)r / 255.0;
    float fg = (float)g / 255.0;
    float fb = (float)b / 255.0;
    float fa = (float)a / 255.0;
    return al_map_rgba_f (fr * fa, fg * fa, fb * fa, fa);
}


