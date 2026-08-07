#ifndef SC_AUTOCLICK_H
#define SC_AUTOCLICK_H

#include <stdbool.h>

#include <SDL3/SDL.h>

#include "controller.h"
#include "coords.h"

struct sc_autoclick {

    struct sc_controller *controller;

    struct sc_position position;

    SDL_Thread *thread;

    SDL_Mutex *mutex;

    bool running;

    int interval_ms;
};

bool
sc_autoclick_init(struct sc_autoclick *ac,
                  struct sc_controller *controller);

void
sc_autoclick_destroy(struct sc_autoclick *ac);

void
sc_autoclick_start(struct sc_autoclick *ac);

void
sc_autoclick_stop(struct sc_autoclick *ac);


#endif