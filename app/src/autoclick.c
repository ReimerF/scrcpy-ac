#include "autoclick.h"

#include <SDL3/SDL.h>

#include "control_msg.h"
#include "controller.h"
#include "input_events.h"

#include "util/log.h"

static void
sc_autoclick_click(struct sc_controller *controller);


bool
sc_autoclick_init(struct sc_autoclick *ac,
                  struct sc_controller *controller) {

    ac->controller = controller;

    ac->running = false;

    ac->interval_ms = 100;

    ac->mutex = SDL_CreateMutex();

    if (!ac->mutex) {
        return false;
    }

    ac->thread = NULL;

    return true;
}

void
sc_autoclick_destroy(struct sc_autoclick *ac) {

    if (ac->running) {
        sc_autoclick_stop(ac);
    }

    if (ac->mutex) {
        SDL_DestroyMutex(ac->mutex);
        ac->mutex = NULL;
    }
}

static int
sc_autoclick_thread(void *userdata)
{
    struct sc_autoclick *ac = userdata;

    SDL_Delay(2000);

    LOGI("Autoclick thread started");
    while (ac->running) {

        sc_autoclick_click(ac->controller);

        SDL_LockMutex(ac->mutex);
        int interval = ac->interval_ms;
        SDL_UnlockMutex(ac->mutex);

        SDL_Delay(interval);
    }

    return 0;
}

void
sc_autoclick_start(struct sc_autoclick *ac) {

    if (ac->running) {
        return;
    }

    ac->running = true;

    sc_autoclick_click(ac->controller);
    ac->thread = SDL_CreateThread(
        sc_autoclick_thread,
        "autoclick",
        ac);

    if (!ac->thread) {
        ac->running = false;
        LOGE("Could not create autoclick thread");
    }
}

void
sc_autoclick_stop(struct sc_autoclick *ac) {

    if (!ac->running) {
        return;
    }

    ac->running = false;

    SDL_WaitThread(ac->thread, NULL);

    ac->thread = NULL;
}

static void
sc_autoclick_click(struct sc_controller *controller) {

    struct sc_control_msg down = {
        .type = SC_CONTROL_MSG_TYPE_INJECT_TOUCH_EVENT,
        .inject_touch_event = {
            .action = AMOTION_EVENT_ACTION_DOWN,
            .pointer_id = SC_POINTER_ID_MOUSE,
            .position = {
                .point = {
                    .x = 720,
                    .y = 1560,
                },
                .screen_size = {
                    .width = 1080,
                    .height = 2340
                },
            },
            .pressure = 1.0f,
            .action_button = AMOTION_EVENT_BUTTON_PRIMARY,
            .buttons = AMOTION_EVENT_BUTTON_PRIMARY,
        },
    };

    struct sc_control_msg up = down;

    up.inject_touch_event.action = AMOTION_EVENT_ACTION_UP;
    up.inject_touch_event.pressure = 0.0f;
    up.inject_touch_event.buttons = 0;

    bool ok = sc_controller_push_msg(controller, &down);

    SDL_Delay(10);

    ok = sc_controller_push_msg(controller, &up);
}