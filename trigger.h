#ifndef LEDCTL_TRIGGER_H
#define LEDCTL_TRIGGER_H

#include <stdlib.h>

#define TRIGGER_BUF_SIZE 2048

int get_trigger(size_t led_number, char *buf, size_t buf_len);
int set_trigger(size_t led_number, const char *trigger);

#endif
