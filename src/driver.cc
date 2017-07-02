#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "strobe.hh"

void update (struct GameState &s) {

    if (s.paused) { return; }

    std::vector< Segment >::iterator SIT = s.segments.begin (), 
        SEND = s.segments.end ();

    al_clear_to_color (al_map_rgb (0, 0, 0));

    s.ref_mark.Draw ();
    s.ref_mark.Tick ();

    if (s.ref_mark.Looped ()) {
        /* TODO: show the stroboscopic image */
    } else {
        for (; SIT != SEND; ++SIT) {
            SIT->Draw (s.ref_mark.Angle ());
        }
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

int main (int argc, char **argv) {
    ALLEGRO_EVENT_QUEUE *events = NULL;
    ALLEGRO_DISPLAY *screen = NULL;
    ALLEGRO_TIMER *update_timer = NULL;
    GameState state;

    float whalf = WIDTH / 2.0,
          hhalf = HEIGHT / 2.0,
          hq = hhalf / 2.0;

    Rectangle capture(WIDTH / 2.0, hhalf - hq, WIDTH, hhalf + hq);
    //state.flash.Set (WIDTH / 2.0, hhalf - hq, WIDTH, hhalf + hq);

    RadialPoint p1(whalf / 2.0, -45.0);
    RadialPoint p2(whalf / 2.0, -60.0);

    state.segments.push_back (Segment (p1, p2));

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
                update (state);
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



