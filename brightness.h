#ifndef LEDCTL_BRIGHTNESS_H
#define LEDCTL_BRIGHTNESS_H
#include <stdlib.h>

int set_brightness(size_t led_number, long brightness);
long get_brightness(size_t led_number);
#endif
