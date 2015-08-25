#ifndef LEDCTL_DEVICE_H
#define LEDCTL_DEVICE_H

#include <stdlib.h>

int read_device_file(size_t led_number, const char *device_file, char *buf,
		     size_t buf_size);
int write_device_file(size_t led_number, const char *device_file, char *buf,
		      size_t buf_size);

/* Pre-formatted input/output */
long read_device_file_long(size_t led_number, const char *device_file);
int write_device_file_long(size_t led_number, const char *device_file,
			   long value);

#endif
