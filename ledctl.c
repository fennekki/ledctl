#include "ledctl.h"

#include "led_info.h"
#include "brightness.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


/* For usage, errors */
char *program_name;


int handle_set_brightness(char *led_str, char *brightness_str) {
    int err;
    char *strtol_endptr;
    long led_number;
    long brightness;

    /* 
     * The process: Set errno to 0. Convert parameter to number. If
     * errno changed, we have an error and should return. If the value
     * strtol_endptr points to does not equal the zero byte, the
     * parameter had trailing characters after a valid number.
     */
    errno = 0;
    led_number = strtol(led_str, &strtol_endptr, 10);

    if (errno != 0) {
        fprintf(stderr, "%s: Invalid argument\n", program_name);
        return -ARGUMENT_ERROR;
    }
    else if (*strtol_endptr != '\0') {
        fprintf(stderr,"%s: Led number must be an integer\n", program_name);
        return -ARGUMENT_ERROR;
    }

    errno = 0;
    brightness = strtol(brightness_str, &strtol_endptr, 10);

    if (errno != 0) {
        fprintf(stderr, "%s: Invalid argument\n", program_name);
        return -ARGUMENT_ERROR;
    }
    else if (*strtol_endptr != '\0') {
        fprintf(stderr,"%s: Brightness must be an integer\n", program_name);
        return -ARGUMENT_ERROR;
    }

    if ((err = set_brightness(led_number, brightness)) < 0) {
        return err;
    }

    return 1;

}

int handle_get_brightness(char *led_str) {
    long ret;
    char *strtol_endptr;
    long led_number;

    errno = 0;
    led_number = strtol(led_str, &strtol_endptr, 10);

    if (errno != 0) {
        fprintf(stderr, "%s: Invalid argument\n", program_name);
        return -ARGUMENT_ERROR;
    }
    else if (*strtol_endptr != '\0') {
        fprintf(stderr,"%s: Led number must be an integer\n", program_name);
        return -ARGUMENT_ERROR;
    }

    if ((ret = get_brightness(led_number)) < 0) {
        return ret;
    }

    fprintf(stdout, "%ld\n", ret);

    return 1;
}


void usage() {
    fprintf(stderr, "usage: %s command [argument...]\n\n"
        "Extraneous arguments are ignored.\n"
        "Valid commands: enumerate\n"
        "                brightness <led> [<brightness>]",
        program_name);
}


/*
 * NOTE: You can just use strstr(...) == haystack for non-literal
 * haystack!
 */
int substring_search_match(const char *haystack, const char *needle) {
    char *strstr_result;

    /* Check if we found a substring at all */
    if ((strstr_result = strstr(haystack, needle))!= NULL) {
        /* Check whether it starts at the beginning */
        if (strcmp(strstr_result, haystack) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int err, command_err;

    /* What a neat hack, eh */
    program_name = argv[0];

    err = 0;
    if (argc > 1) {
        /* Start by creating the indices */
        if ((command_err = build_led_index() < 0)) {
            return -command_err;
        }

        /*
         * Incredible hack: If a substring is found, see if its start
         * location matches the start location matches the start of the
         * string "enumerate"
         */
        if (substring_search_match("enumerate", argv[1]) != 0) {
            enumerate_leds();
        }
        else if (substring_search_match("brightness", argv[1]) != 0) {
            if (argc > 3) {
                if ((command_err = handle_set_brightness(argv[2], argv[3]))
                        < 0) {
                    err = -command_err;
                }
            }
            else if (argc == 3) {
                if ((command_err = handle_get_brightness(argv[2])) < 0) {
                    err = -command_err;
                }
            }
            else {
                fprintf(stderr, "%s: Too few parameters\n", program_name);
                err = ARGUMENT_ERROR;
            }
        }
        else {
            usage();
            err = INVALID_COMMAND_ERROR;
        }

        /* Free the led index */
        destroy_led_index();
    }
    else {
        usage();
        err = INVALID_COMMAND_ERROR;
    }

    return err;
}
