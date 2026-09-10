// Copyright 2025 Ira Cooper <ira@wakeful.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "viable.h"
#include "quantum.h"

#ifdef TAP_DANCE_ENABLE

#    include "process_tap_dance.h"

// Dance state tracking
enum { SINGLE_TAP = 1, SINGLE_HOLD, DOUBLE_TAP, DOUBLE_HOLD, DOUBLE_SINGLE_TAP, MORE_TAPS };

static uint8_t                  dance_state[VIABLE_TAP_DANCE_ENTRIES];
static viable_tap_dance_entry_t td_entry;

static uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed)
            return SINGLE_TAP;
        else
            return SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted)
            return DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return DOUBLE_HOLD;
        else
            return DOUBLE_TAP;
    }
    return MORE_TAPS;
}

static void on_dance(tap_dance_state_t *state, void *user_data) {
    uint8_t index = (uintptr_t)user_data;
    if (viable_get_tap_dance(index, &td_entry) != 0) return;
    if (!TD_ENABLED(td_entry)) return;
    uint16_t kc = td_entry.on_tap;
    if (kc) {
        if (state->count == 3) {
            viable_keycode_tap(kc);
            viable_keycode_tap(kc);
            viable_keycode_tap(kc);
        } else if (state->count > 3) {
            viable_keycode_tap(kc);
        }
    }
}

static void on_dance_finished(tap_dance_state_t *state, void *user_data) {
    uint8_t index = (uintptr_t)user_data;
    if (viable_get_tap_dance(index, &td_entry) != 0) return;
    if (!TD_ENABLED(td_entry)) return;
    dance_state[index] = dance_step(state);
    switch (dance_state[index]) {
        case SINGLE_TAP: {
            if (td_entry.on_tap) viable_keycode_down(td_entry.on_tap);
            break;
        }
        case SINGLE_HOLD: {
            if (td_entry.on_hold)
                viable_keycode_down(td_entry.on_hold);
            else if (td_entry.on_tap)
                viable_keycode_down(td_entry.on_tap);
            break;
        }
        case DOUBLE_TAP: {
            if (td_entry.on_double_tap) {
                viable_keycode_down(td_entry.on_double_tap);
            } else if (td_entry.on_tap) {
                viable_keycode_tap(td_entry.on_tap);
                viable_keycode_down(td_entry.on_tap);
            }
            break;
        }
        case DOUBLE_HOLD: {
            if (td_entry.on_tap_hold) {
                viable_keycode_down(td_entry.on_tap_hold);
            } else {
                if (td_entry.on_tap) {
                    viable_keycode_tap(td_entry.on_tap);
                    if (td_entry.on_hold)
                        viable_keycode_down(td_entry.on_hold);
                    else
                        viable_keycode_down(td_entry.on_tap);
                } else if (td_entry.on_hold) {
                    viable_keycode_down(td_entry.on_hold);
                }
            }
            break;
        }
        case DOUBLE_SINGLE_TAP: {
            if (td_entry.on_tap) {
                viable_keycode_tap(td_entry.on_tap);
                viable_keycode_down(td_entry.on_tap);
            }
            break;
        }
    }
}

static void on_dance_reset(tap_dance_state_t *state, void *user_data) {
    uint8_t index = (uintptr_t)user_data;
    if (viable_get_tap_dance(index, &td_entry) != 0) return;
    if (!TD_ENABLED(td_entry)) return;
    wait_ms(TAP_CODE_DELAY);
    uint8_t st         = dance_state[index];
    state->count       = 0;
    dance_state[index] = 0;
    switch (st) {
        case SINGLE_TAP: {
            if (td_entry.on_tap) viable_keycode_up(td_entry.on_tap);
            break;
        }
        case SINGLE_HOLD: {
            if (td_entry.on_hold)
                viable_keycode_up(td_entry.on_hold);
            else if (td_entry.on_tap)
                viable_keycode_up(td_entry.on_tap);
            break;
        }
        case DOUBLE_TAP: {
            if (td_entry.on_double_tap) {
                viable_keycode_up(td_entry.on_double_tap);
            } else if (td_entry.on_tap) {
                viable_keycode_up(td_entry.on_tap);
            }
            break;
        }
        case DOUBLE_HOLD: {
            if (td_entry.on_tap_hold) {
                viable_keycode_up(td_entry.on_tap_hold);
            } else {
                if (td_entry.on_tap) {
                    if (td_entry.on_hold)
                        viable_keycode_up(td_entry.on_hold);
                    else
                        viable_keycode_up(td_entry.on_tap);
                } else if (td_entry.on_hold) {
                    viable_keycode_up(td_entry.on_hold);
                }
            }
            break;
        }
        case DOUBLE_SINGLE_TAP: {
            if (td_entry.on_tap) {
                viable_keycode_up(td_entry.on_tap);
            }
            break;
        }
    }
}

// Storage for tap dance actions
static tap_dance_action_t viable_tap_dance_actions[VIABLE_TAP_DANCE_ENTRIES];

void viable_reload_tap_dance(void) {
    for (size_t i = 0; i < VIABLE_TAP_DANCE_ENTRIES; ++i) {
        viable_tap_dance_actions[i].fn.on_each_tap       = on_dance;
        viable_tap_dance_actions[i].fn.on_dance_finished = on_dance_finished;
        viable_tap_dance_actions[i].fn.on_reset          = on_dance_reset;
        viable_tap_dance_actions[i].fn.on_each_release   = NULL;
        viable_tap_dance_actions[i].user_data            = (void *)(uintptr_t)i;
    }
}

// Override the introspection functions
uint16_t tap_dance_count(void) {
    return VIABLE_TAP_DANCE_ENTRIES;
}

tap_dance_action_t *tap_dance_get(uint16_t tap_dance_idx) {
    if (tap_dance_idx >= VIABLE_TAP_DANCE_ENTRIES) {
        return NULL;
    }
    return &viable_tap_dance_actions[tap_dance_idx];
}

// Stub - tap dance processing is handled by QMK's standard mechanism
bool process_record_viable_tap_dance(uint16_t keycode, keyrecord_t *record) {
    return true; // Continue processing
}

#else
// Stubs when TAP_DANCE_ENABLE is not defined
void viable_reload_tap_dance(void) {}
bool process_record_viable_tap_dance(uint16_t keycode, keyrecord_t *record) {
    return true;
}
#endif
