#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "strobe.hh"

Point RadialPoint::GetXY (float angle) const {
    float x = cx_ + radius_ * cos (RADIANS(angle_ + angle));
    float y = cy_ + radius_ * sin (RADIANS(angle_ + angle));
    return Point(x,y);
}

void Segment::Draw (float angle, ALLEGRO_COLOR col) const {
    Point p1 = p1_.GetXY (angle);
    Point p2 = p2_.GetXY (angle);
    al_draw_line (p1.GetX (), p1.GetY (), p2.GetX (), p2.GetY (), col, 1);
}

void Rectangle::Draw (ALLEGRO_COLOR col) const {
    al_draw_filled_rectangle (p1_.GetX (), p1_.GetY(),
            p2_.GetX (), p2_.GetY (), col);
}

bool Rectangle::Contains (const Segment &seg, float angle) const {
    Point start = seg.GetStart ().GetXY (angle);
    Point end = seg.GetEnd ().GetXY (angle);
    bool start_in = false, end_in = false;
    float x = start.GetX (), y = start.GetY ();

    start_in = ((x >= p1_.GetX () && x <= p2_.GetX ()) && 
            (y >= p1_.GetY () && y <= p2_.GetY ()));
    x = end.GetX ();
    y = end.GetY ();
    end_in = ((x >= p1_.GetX () && x <= p2_.GetX ()) && 
            (y >= p1_.GetY () && y <= p2_.GetY ()));
    return (start_in || end_in);
}


void Reference::Draw (ALLEGRO_COLOR col) const {
    tick_.Draw (Angle (), col);
}

void Reference::Tick () {
    angle_ += ANGLE_DELTA;
    if (angle_ >= ANGLE_MAX) {
        angle_ -= ANGLE_MAX;
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


