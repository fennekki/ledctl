#include "brightness.h"

#include "ledctl.h"
#include "led_info.h"
#include "device.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>


long get_max_brightness(size_t led_number) {
    return read_device_file_long(led_number, "max_brightness");
}

int set_brightness(size_t led_number, long brightness) {
    long max_brightness;

    if ((max_brightness = get_max_brightness(led_number)) < 0) {
        return -ARGUMENT_ERROR;
    }

    if (brightness > max_brightness || brightness < 0) {
        fprintf(stderr, "%s: Brightness must be between 0 and %ld\n",
                program_name, max_brightness);
        return -ARGUMENT_ERROR;
    }

    return write_device_file_long(led_number, "brightness", brightness);

}

long get_brightness(size_t led_number) {
    return read_device_file_long(led_number, "brightness");
}
