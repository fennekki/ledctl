#ifndef LEDCTL_LED_INFO_H
#define LEDCTL_LED_INFO_H

#include <stdlib.h>

#define LED_CLASS_PATH "/sys/class/leds/"

#define LED_INFO_FIELD_SIZE 64
#define LED_INFO_FILENAME_SIZE 255
#define INITIAL_LED_INDEX_SIZE 8

struct led_info {
    char device_name[LED_INFO_FIELD_SIZE];
    char color[LED_INFO_FIELD_SIZE];
    char function[LED_INFO_FIELD_SIZE];
    char filename[LED_INFO_FILENAME_SIZE];
};

void get_led_info(const char *const filename,
    char *device_name_out, size_t device_name_out_size,
    char *color_out, size_t color_out_size,
    char *function_out, size_t function_out_size);

int build_led_index();
void destroy_led_index();

void enumerate_leds();

struct led_info *get_led(size_t led_number);

#endif
