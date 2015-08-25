#include "led_info.h"

#include "ledctl.h"

#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>

static struct led_info *led_index;
static size_t led_index_size;
static size_t led_count;

/* For get_led_info */
#define COPY_VALUE_CHECKED(value, value_out, value_out_size) do {\
    if (value_out != NULL && value_out_size > 0) {\
        if (value != NULL) { strncpy(value_out, value, value_out_size);\
        }\
        else {\
            /* Equals copying an empty string! */\
            value_out[0] = '\0';\
        }\
    }\
} while(0)

/* What a monster of a function */
void get_led_info(const char *const filename,
        char *device_name_out, size_t device_name_out_size,
        char *color_out, size_t color_out_size,
        char *function_out, size_t function_out_size) {
    char curr_file_name[LED_INFO_FILENAME_SIZE];
    char *first_tok, *second_tok, *third_tok;
    char *device_name, *color, *function;

    /* Copy filename for strtoking */
    strncpy(curr_file_name, filename, sizeof(curr_file_name));

    /* This needs a hack */
    first_tok = strtok(curr_file_name, ":");
    second_tok = strtok(NULL, ":");
    third_tok = strtok(NULL, ":");

    /*
     * Here's the hack. It's ugly. At least we can
     * assume that we skipped no more than two tokens.
     */

    /* Did we skip any columns? */
    if (first_tok != curr_file_name) {
        /* Did we skip two? */
        if (first_tok > curr_file_name + 1) {
            device_name = NULL;
            color = NULL;
            function = first_tok;
        }
        /* We skipped one */
        else {
            device_name = NULL;
            color = first_tok;
            function = second_tok;
        }
    }
    /* Did we skip the SECOND column? */
    else if (second_tok > curr_file_name + strlen(first_tok) + 1) {
        device_name = first_tok;
        color = NULL;
        function = second_tok;
    }
    else {
        device_name = first_tok;
        color = second_tok;
        function = third_tok;
    }

    /*
     * Output.
     *
     * Every single one checks:
     * - Whether output is NULL -> Not copied
     * - Whether output size is under 1 -> Not copied
     * - Whether the value is NULL -> Copy ""
     * And finally just strcopies it. Note that the copied string
     * might not contain the entire name!!
     *
     * I wrote a macro for this because it's easier that way.
     */

    COPY_VALUE_CHECKED(device_name, device_name_out, device_name_out_size);
    COPY_VALUE_CHECKED(color, color_out, color_out_size);
    COPY_VALUE_CHECKED(function, function_out, function_out_size);
}

int build_led_index() {
    DIR *led_class;
    struct dirent *curr_dirent;
    struct led_info *led;
    struct led_info *old_led_index;
    size_t old_led_index_size;

    led_class = opendir(LED_CLASS_PATH);
    led_index = calloc(INITIAL_LED_INDEX_SIZE, sizeof(struct led_info));
    led_index_size = INITIAL_LED_INDEX_SIZE;
    led_count = 0;

    /* Could error if there are no leds, I suppose */
    if (led_class == NULL) {
        fprintf(stderr, "%s: Could not find any LEDs\n", program_name);
        return -LED_INDEX_ERROR;
    }

    while ((curr_dirent = readdir(led_class)) > 0) {
        /* Ignore ., .., "hidden files" */
        if (curr_dirent->d_name[0] != '.') {
            /* Increment led count */
            ++led_count;

            /* Check whether we can fit this amount of leds*/
            if (led_index_size < led_count) {
                old_led_index = led_index;
                old_led_index_size = led_index_size;

                /* If we can't, double the size of the index */
                led_index_size = old_led_index_size * 2;
                led_index = calloc(led_index_size, sizeof(struct led_info));

                /* Copy old data over */
                memcpy(led_index, old_led_index,
                       old_led_index_size * sizeof(struct led_info));

                /* Free old data */
                free(old_led_index);
            }

            /* Get pointer to current led */
            led = led_index + led_count - 1;

            strncpy(led->filename, curr_dirent->d_name, sizeof(led->filename));
            /* Just... look at this function. */
            get_led_info(curr_dirent->d_name,
                led->device_name, sizeof(led->device_name),
                led->color, sizeof(led->color),
                led->function, sizeof(led->function));
        }
    }

    /* Handle error? */
    if (curr_dirent < 0) {
        fprintf(stderr, "%s: Some LEDs could not be enumerated\n",
            program_name);
    }

    closedir(led_class);

    return 1;
}

void destroy_led_index() {
    free(led_index);
}


void enumerate_leds() {
    unsigned long i;
    struct led_info *led;
    for (i = 0; i < led_count; ++i) {
        led = led_index + i;
        fprintf(stdout,
            "%lu\tDevice name: %s\tColor: %s\tFunction: %s\n",
            i, led->device_name, led->color, led->function);
    }
};

struct led_info *get_led(size_t led_number) {
    if (led_number < led_count) {
        return led_index + led_number;
    }
    else {
        return NULL;
    }
}
