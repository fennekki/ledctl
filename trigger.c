#include "trigger.h"

#include "ledctl.h"
#include "led_info.h"
#include "device.h"

#include <stdio.h>
#include <string.h>

int get_triggers(size_t led_number, char *buf, size_t buf_len) {
    return read_device_file(led_number, "trigger", buf, buf_len);
}

int get_trigger(size_t led_number, char *buf, size_t buf_len) {
    int err;
    char *active;
	
    if ((err = get_triggers(led_number, buf, buf_len)) < 0) {
        return err;
    }

    /* Make sure trigger ends with a zero byte */
    buf[buf_len - 1] = '\0';

    /* Find active trigger */
    active = strstr(buf, "[") + 1;
    /* strtok to extract ONLY active trigger */
    active = strtok(active, "[]");

    /* Move to beginning of buf, including ¸'\0' */
    memmove(buf, active, strlen(active) + 1);

    return 1;
}

int set_trigger(size_t led_number, const char *trigger) {
    int err;
    char buf[TRIGGER_BUF_SIZE];
    char *tmp;
    char *err_tmp;

    memset(buf, '\0', sizeof(buf));
    
    if ((err = get_triggers(led_number, buf, sizeof(buf)) < 0)) {
        return err;
    }

    if ((tmp = strstr(buf, trigger)) == NULL) {
        fprintf(stderr, "%s: trigger must be one of %s\n", program_name,
		buf);
        return -ARGUMENT_ERROR;
    }

    if (strncmp(tmp, trigger, strlen(trigger)) != 0) {
        fprintf(stderr, "%s: trigger must be one of %s\n", program_name,
                buf);
        return -ARGUMENT_ERROR;
    }

    /* Reassign tmp to end of trigger */
    tmp = tmp + strlen(trigger);

    /* Allowed characters after the string are these, I think */
    if (*tmp != '\0' && *tmp != ' ' && *tmp != ']') {
        fprintf(stderr, "%s: trigger must be one of %s\n", program_name,
                buf);
        return -ARGUMENT_ERROR;
    }

    return write_device_file(led_number, "trigger", trigger, strlen(trigger));
}
