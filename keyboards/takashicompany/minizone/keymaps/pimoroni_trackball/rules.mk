POINTING_DEVICE_ENABLE = yes
# POINTING_DEVICE_DRIVER = pimoroni_trackball
SRC += drivers/sensors/pimoroni_trackball.c
QUANTUM_LIB_SRC += i2c_master.c

OLED_ENABLE = no
# VIA_ENABLE = yes