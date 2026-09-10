// Copyright 2025 Ira Cooper <ira@wakeful.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Include generated config from viable.json (defines VIABLE_*_ENTRIES)
#include "viable_config.h"

// Defaults for features not in viable.json (0 = disabled)
#ifndef VIABLE_TAP_DANCE_ENTRIES
#    define VIABLE_TAP_DANCE_ENTRIES 0
#endif

#ifndef VIABLE_COMBO_ENTRIES
#    define VIABLE_COMBO_ENTRIES 0
#endif

#ifndef VIABLE_KEY_OVERRIDE_ENTRIES
#    define VIABLE_KEY_OVERRIDE_ENTRIES 0
#endif

#ifndef VIABLE_ALT_REPEAT_KEY_ENTRIES
#    define VIABLE_ALT_REPEAT_KEY_ENTRIES 0
#endif

#ifndef VIABLE_LEADER_ENTRIES
#    define VIABLE_LEADER_ENTRIES 0
#endif

// Route QMK's tap-hold decisions through the per-key callbacks implemented in
// viable_qmk_settings.c so the values stored via QMK Settings (tapping term,
// permissive hold, hold-on-other-key-press, quick tap term) take effect at
// runtime. These must be C defines, not rules.mk variables: without them QMK
// compiles the callbacks out and uses the compile-time constants instead.
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define QUICK_TAP_TERM_PER_KEY

// Compile in the remaining Tap-Hold features whose toggles QMK Settings
// exposes (Chordal Hold, Flow Tap, Retro Tapping). Each decision is routed
// through a callback in viable_qmk_settings.c that honours the stored setting;
// with the setting off, behaviour is the same as the feature not being built.
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 0 // placeholder: the runtime value comes from get_flow_tap_term()
#define RETRO_TAPPING_PER_KEY

// QMK settings storage size (viable_qmk_settings_t)
#define VIABLE_QMK_SETTINGS_SIZE 44

// Dynamic leader timeout - Viable controls this variable
#if defined(LEADER_ENABLE) && !defined(__ASSEMBLER__)
#    include <stdint.h>
extern uint16_t viable_leader_timeout;
#    define LEADER_TIMEOUT (viable_leader_timeout)
#endif

// Total size: tap_dance*10 + combo*12 + key_override*12 + alt_repeat*6 + one_shot(3) + leader*14 + magic(6) + qmk_settings(44) + fragments(21)
#define VIABLE_EEPROM_SIZE_CALC ((VIABLE_TAP_DANCE_ENTRIES * 10) + (VIABLE_COMBO_ENTRIES * 12) + (VIABLE_KEY_OVERRIDE_ENTRIES * 12) + (VIABLE_ALT_REPEAT_KEY_ENTRIES * 6) + 3 + (VIABLE_LEADER_ENTRIES * 14) + 6 + VIABLE_QMK_SETTINGS_SIZE + 21)

#ifndef EECONFIG_KB_DATA_SIZE
#    define EECONFIG_KB_DATA_SIZE VIABLE_EEPROM_SIZE_CALC
#endif
