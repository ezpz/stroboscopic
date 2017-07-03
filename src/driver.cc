#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "strobe.hh"

#define TARGET_THETA 60.0

std::vector< Segment > captured;

void check_overlap (const std::vector< Segment > &segments, 
        const Rectangle& flash, float theta) {
    if (static_cast< long >(theta) % static_cast< long >(TARGET_THETA)) { 
        return; /* not on the period of the flash */
    }
    std::vector< Segment >::const_iterator SIT = segments.begin (),
        SEND = segments.end ();
    for (; SIT != SEND; ++SIT) {
        if (flash.Contains (*SIT, theta)) {
            captured.push_back (SIT->SaveAt (theta));
        }
    }
}

void update (struct GameState &s, const Rectangle &flash) {

    if (s.paused) { return; }

    al_clear_to_color (al_map_rgb (0, 0, 0));

    s.ref_mark.Draw ();
    s.ref_mark.Tick ();

    flash.Draw ();

    if (s.ref_mark.Looped ()) {
        std::vector< Segment >::iterator SIT = captured.begin (), 
            SEND = captured.end ();
        for (; SIT != SEND; ++SIT) {
            SIT->Draw (0, mkcol (255, 69, 0, 255));
        }
        s.paused = true;
        captured.clear ();
    } else {
        std::vector< Segment >::iterator SIT = s.segments.begin (), 
            SEND = s.segments.end ();
        for (; SIT != SEND; ++SIT) {
            SIT->Draw (s.ref_mark.Angle ());
        }
        check_overlap (s.segments, flash, s.ref_mark.Angle ());
    }

    s.border.Draw ();

    al_flip_display ();
}

void handle_key_down (const ALLEGRO_EVENT &event, GameState &state) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            state.running = false;
        }
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
            state.paused = ! state.paused;
        }
    }
}

/*
 * theta is the angle step at which the capture flash will occur
 */
void generate_shape (std::vector< Segment > &segments, float theta) {
    /*
     * Simple test of a 'box' described in cartesian coordinates
     * and randomized in polar 
     */

    float step = 20.0, xoff = 100.0;
    Point p1, p2;

    /* bottom */
    for (int i = 0; i < 4; ++i) {
        p1.Set (xoff + step * i,         0.0);
        p2.Set (xoff + step * (i + 1),   0.0);
        segments.push_back (Segment (p1.GetPolar (), p2.GetPolar ()));
    }

    /* top */
    for (int i = 0; i < 4; ++i) {
        p1.Set (xoff + step * i,         -4 * step);
        p2.Set (xoff + step * (i + 1),   -4 * step);
        segments.push_back (Segment (p1.GetPolar (), p2.GetPolar ()));
    }

    /* left */
    for (int i = 0; i < 4; ++i) {
        p1.Set (xoff, -step * i);
        p2.Set (xoff, -step * (i + 1));
        segments.push_back (Segment (p1.GetPolar (), p2.GetPolar ()));
    }


    /* right */
    for (int i = 0; i < 4; ++i) {
        p1.Set (xoff + 4 * step, -step * i);
        p2.Set (xoff + 4 * step, -step * (i + 1));
        segments.push_back (Segment (p1.GetPolar (), p2.GetPolar ()));
    }


    /* iterate through all segments and rotate them along some multiple
     * along the rotation
     */
    std::vector< Segment >::iterator SIT = segments.begin (),
        SEND = segments.end ();
    for (; SIT != SEND; ++SIT) {
        int n = rand () % static_cast< int >(360.0 / theta);
        SIT->Rotate (n * theta);
    }
}

int main (int argc, char **argv) {
    ALLEGRO_EVENT_QUEUE *events = NULL;
    ALLEGRO_DISPLAY *screen = NULL;
    ALLEGRO_TIMER *update_timer = NULL;
    GameState state;

    float whalf = WIDTH / 2.0,
          hhalf = HEIGHT / 2.0,
          hq = hhalf / 2.0;

    Rectangle capture(WIDTH / 2.0, hhalf - hq, WIDTH, hhalf + hq);

    generate_shape (state.segments, TARGET_THETA);

    if (! al_init ()) {
        fprintf (stderr, "Failed to init allegro\n");
        return 1;
    }

    screen = al_create_display (WIDTH, HEIGHT);
    if (NULL == screen) {
        fprintf (stderr, "Failed to create display\n");
        return 1;
    }

    events = al_create_event_queue ();
    if (NULL == events) {
        fprintf (stderr, "Failed to create event queue\n");
        return 1;
    }

    //update_timer = al_create_timer (1.0 / FPS);
    update_timer = al_create_timer (5.0 / FPS);
    if (NULL == update_timer) {
        fprintf (stderr, "Failed to create update timer\n");
        return 1;
    }

    al_init_primitives_addon ();
    al_install_keyboard ();
    al_register_event_source (events, al_get_display_event_source (screen));
    al_register_event_source (events, al_get_keyboard_event_source ());
    al_register_event_source (events, 
            al_get_timer_event_source (update_timer));

    al_start_timer (update_timer);

    while (state.running) {

        ALLEGRO_EVENT event;
        al_wait_for_event (events, &event);

        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                update (state, capture);
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                state.running = false;
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                handle_key_down (event, state);
                break;
            default:
                /* */
                break;
        }

    }

    return 0;
}



