#ifndef SC_AUTOCLICK_H
#define SC_AUTOCLICK_H

#include "controller.h"

struct sc_autoclick {
    struct sc_controller *controller;
};

void
sc_autoclick_click(struct sc_controller *controller);

#endif