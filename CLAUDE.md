# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a fork of QMK Firmware focused on the **Svalboard** keyboard with the **Viable** module. The Svalboard is a split ergonomic keyboard with integrated pointing devices (trackball, trackpoint, or Azoteq touch).

## Build Commands

Build firmware for Svalboard:
```bash
make svalboard/left:viable          # Left half with viable keymap
make svalboard/right:viable         # Right half with viable keymap
make svalboard/trackball/pmw3360/left:viable   # Trackball variant
make svalboard/trackpoint/left:viable          # Trackpoint variant
make svalboard/azoteq/left:viable              # Azoteq variant
```

Flash firmware:
```bash
make svalboard/left:viable:flash
```

Run unit tests:
```bash
make test:all                       # Run all tests
make test:basic                     # Run specific test suite
```

Clean build artifacts:
```bash
make clean                          # Remove .build directory
make distclean                      # Also remove firmware files (.bin, .hex, .uf2)
```

## Architecture

### Directory Structure

- `keyboards/svalboard/` - Svalboard-specific keyboard code
  - `left/`, `right/` - Split halves with keyboard.json configs
  - `trackball/`, `trackpoint/`, `azoteq/` - Pointing device variants
  - `keymaps/` - Keymap definitions (viable, blank, default)
  - `keymaps/keymap_support.c` - Shared keymap utilities
- `modules/viable-kb/core/` - Viable module for dynamic configuration via USB HID
- `quantum/` - QMK core firmware features
- `platforms/` - Platform-specific code (RP2040, AVR, ARM)
- `drivers/` - Hardware drivers
- `builddefs/` - Build system makefiles

### Svalboard Architecture

The Svalboard uses a split keyboard design with:
- **Matrix scanning** via `matrix.c` with configurable scan timing (`turbo_scan`)
- **Pointing devices** - Each half can have trackball (PMW3360/PMW3389), trackpoint, or Azoteq
- **RGB layer indicators** - Per-layer HSV colors stored in EEPROM
- **VIA/Vial protocol** - Custom configuration through `via_custom_value_command_kb()`
- **Split sync** via QMK's transaction RPC (`KEYBOARD_SYNC_A`)

Key files:
- `svalboard.c` - Main keyboard logic, EEPROM handling, VIA integration
- `svalboard.h` - Shared types including `saved_values_t` for persistent settings
- `axis_scale.c` - Pointer axis scaling/calibration

### Viable Module

The Viable module (`modules/viable-kb/core/`) provides dynamic QMK feature configuration:
- Tap dance, combos, key overrides, alt repeat keys
- QMK settings (tapping term, permissive hold, etc.)
- Uses USB HID protocol with `0xDF` prefix
- Stores data in EEPROM with build-timestamp-based validation

To use Viable in a keymap, add to `keymap.json`:
```json
{ "modules": ["viable-kb/core"] }
```

### Build System

The build uses QMK's make system with these key patterns:
- `make keyboard:keymap[:target]` - Standard build format
- Parallel builds work at the keyboard level, not top-level make
- Build artifacts go to `.build/` directory
- Python 3 and `qmk` CLI are required dependencies

### CI/CD

GitHub Actions workflows:
- `release.yml` - Builds all Svalboard variants on push/tag
- `build-firmware.yml` - Reusable workflow for building specific keyboard/keymap combinations
- Produces `.uf2` firmware files as artifacts or releases

## Key Patterns

### Custom Keycodes

Svalboard defines custom keycodes like `SV_OUTPUT_STATUS`, `SV_LEFT_DPI_INC`, `SV_SNIPER_3` for keyboard-specific functions.

### Layer Management

Uses `sval_set_active_layer()` to sync layer state with RGB indicators across split halves.

### EEPROM Validation

Both Svalboard and Viable use build timestamp as a magic number to detect firmware updates and reset settings appropriately.
