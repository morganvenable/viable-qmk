POINTING_DEVICE_ENABLE = yes
# Use custom wrapper that calls azoteq functions directly
POINTING_DEVICE_DRIVER = custom

# Include Azoteq driver sources for our custom wrapper to call
SRC += drivers/sensors/azoteq_iqs5xx.c
QUANTUM_LIB_SRC += i2c_master.c

EXTRA_SRC = azoteq/azoteq.c

# TPS43 0.7 mm glass calibration: IQS5xx I2C bootloader programmer.
# Bare filename: QMK puts the keyboard directories on VPATH. (azoteq.c itself
# is picked up automatically by the per-directory-level naming convention --
# EXTRA_SRC above is vestigial, nothing in the build system reads it.)
SRC += iqs5xx_bootloader.c

MH_AUTO_BUTTONS = yes