#include "ledctl.h"
#include "led_info.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int read_device_file(size_t led_number, const char *device_file, char *buf,
                     size_t buf_size) {
    int err;
    struct led_info *led;
    char filename[LED_INFO_FILENAME_SIZE * 2];
    int fd;

    err = 0;
    led = get_led(led_number);
    
    if (led == NULL) {
        fprintf(stderr, "%s: Invalid LED number\n", program_name);
        return -ARGUMENT_ERROR;
    }

    snprintf(filename, sizeof(filename), LED_CLASS_PATH "%s/%s",
             led->filename, device_file);

    fd = open(filename, O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "%s: Could not open device file %s\n",
                program_name, device_file);
        return -IO_FAILURE_ERROR;
    }

    if (read(fd, buf, buf_size) < 0) {
        /* Read error */
        fprintf(stderr, "%s: Could not read max brightness\n", program_name);
        err = -IO_FAILURE_ERROR;
    }

    close(fd);

    if (err != 0) {
        return err;
    }

    return 1;

}

int write_device_file(size_t led_number, const char *device_file,
                      const char *buf, size_t buf_size) {
    int err;
    struct led_info *led;
    char filename[LED_INFO_FILENAME_SIZE * 2];
    int fd;

    err = 0;
    led = get_led(led_number);
    
    if (led == NULL) {
        fprintf(stderr, "%s: Invalid LED number\n", program_name);
        return -ARGUMENT_ERROR;
    }

    snprintf(filename, sizeof(filename), LED_CLASS_PATH "%s/%s",
             led->filename, device_file);

    fd = open(filename, O_WRONLY);

    /* In case we didn't get a file descriptor */
    if (fd < 0) {
        fprintf(stderr, "%s: Could not open device file %s\n",
                program_name, device_file);
        return -IO_FAILURE_ERROR;
    }


    /* Used to use sizeof here --> extra data. DURRRR */
    if (write(fd, buf, buf_size) < 0) {
        fprintf(stderr, "%s: Could not write to device file %s\n",
                program_name, device_file);
        err = -IO_FAILURE_ERROR;
    }

    close(fd);

    if (err != 0) {
        return err;
    }
    
    return 1;
}

/* A wrapper for read_device_file that does the conversion */
long read_device_file_long(size_t led_number, const char *device_file) {
    int err;
    char *strtol_endptr;
    /* More than the length of 2^64 */
    char ret_str[22];
    long ret;

    /* Initialise memory block */
    memset(ret_str, '\0', sizeof(ret_str));

    if ((err = read_device_file(led_number, device_file, ret_str,
                                sizeof(ret_str))) < 0) {
        return err;
    }
    else {
        /* This should never error but, what the hell */
        ret = strtol(ret_str, &strtol_endptr, 10);

        if (errno != 0) {
            fprintf(stderr, "%s: Device file returned invalid value\n",
                    program_name);
            return -UNKNOWN_ERROR;
        }
        else if (*strtol_endptr != '\n') {
            fprintf(stderr, "%s: Device file returned garbage data\n",
                    program_name);
            return -UNKNOWN_ERROR;
        }

        return ret;
    }
}

int write_device_file_long(size_t led_number, const char* device_file,
                           long value) {
    int err;
    char value_str[22];

    snprintf(value_str, sizeof(value_str), "%ld", value);
    if ((err = write_device_file(led_number, device_file, value_str,
                          sizeof(value_str)) < 0)) {
        return err;
    }

    return 1;
}
