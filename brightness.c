#include "brightness.h"

#include "ledctl.h"
#include "led_info.h"

#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

long read_max_brightness(size_t led_number) {
	int err;
	char *strtol_endptr;
	struct led_info *led;
	char filename[LED_INFO_FILENAME_SIZE * 2];
	/* More than the length of 2^64 */
	char brightness_str[20];
	long brightness;
	int fd;

	err = 0;

	led = led_index + led_number;

	snprintf(filename, sizeof(filename),
		LED_CLASS_PATH "%s/max_brightness",
		led->filename);

	fd = open(filename, O_RDONLY);

	if (fd < 0) {
		fprintf(stderr, "%s: Could not open max brightness file\n",
			program_name);
		return -2;
	}

	if (read(fd, brightness_str, sizeof(brightness_str)) > 0) {
		errno = 0;
		brightness = strtol(brightness_str, &strtol_endptr, 10);

		if (errno != 0) {
			fprintf(stderr, "%s: Invalid value\n",
				program_name);
			/* Argument error... somehow */
			return -3;
		}

	}
	else {
		/* Read error */
		fprintf(stderr, "%s: Could not read max brightness\n",
			program_name);
		err = -2;
	}
	
	close(fd);
	
	if (err != 0) {
		return err;
	}
	else {
		return brightness;
	}
}

int set_brightness(size_t led_number, long brightness) {
	int err;
	struct led_info *led;
	char filename[LED_INFO_FILENAME_SIZE * 2];
	long max_brightness;
	char brightness_str[20];
	int fd;

	err = 0;
	if ((max_brightness = read_max_brightness(led_number)) < 0) {
		err = -1;
	}
	else {

		if (brightness > max_brightness || brightness < 0) {
			fprintf(stderr,
				"%s: Brightness must be between 0 and %ld\n",
				program_name, max_brightness);
			return -1;
		}

		led = led_index + led_number;

		snprintf(filename, sizeof(filename),
			LED_CLASS_PATH "%s/brightness",
			led->filename);

		fd = open(filename, O_WRONLY);

		/* In case we didn't get a file descriptor */
		if (fd < 0) {
			fprintf(stderr, "%s: Could not open brightness file\n",
				program_name);
			return -1;
		}

		snprintf(brightness_str, sizeof(brightness_str), "%ld",
			brightness);

		if (write(fd, brightness_str, sizeof(brightness_str)) < 0) {
			fprintf(stderr, "%s: Could not write brightness\n",
				program_name);
			err = -2;
		}

		close(fd);
	}

	return err;
}
