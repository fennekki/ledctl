#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>

static const char* led_class_path = "/sys/class/leds/";
static char *program_name;
static char error_string[255];

/* Macro'd for convenience */
#define PRINT_ERROR(filename_or_whatever) do {\
	snprintf(error_string, sizeof(error_string), "%s: %s",\
		program_name, filename_or_whatever);\
	perror(error_string);\
} while(0)

/* For get_led_info */
#define COPY_VALUE_CHECKED(value, value_out, value_out_size) do {\
	if (value_out != NULL && value_out_size > 0) {\
		if (value != NULL) {\
			strncpy(value_out, value, value_out_size);\
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
	char curr_file_name[255];
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

void enumerate_leds() {
	DIR *led_class;
	struct dirent *curr_dirent;
	char led_devicename[64], led_color[64], led_function[64];

	led_class = opendir(led_class_path);

	/* Could error if there are no leds, I suppose */
	if (led_class == NULL) {
		PRINT_ERROR(led_class_path);
		return;
	}
	
	while ((curr_dirent = readdir(led_class)) > 0) {
		/* Ignore ., .., "hidden files" */
		if (curr_dirent->d_name[0] != '.') {
			/* Just... look at this function. */
			get_led_info(curr_dirent->d_name,
				led_devicename, sizeof(led_devicename),
				led_color, sizeof(led_color),
				led_function, sizeof(led_function));

			fprintf(stdout, "Name: %s\tColor: %s \tFunction:%s\n",
				led_devicename, led_color, led_function);
		}
	}

	/* Handle error? */
	if (curr_dirent < 0) {
		perror(program_name);
	}

	closedir(led_class);
}

int main(int argc, char *argv[])
{
	/* What a neat hack, eh */
	program_name = argv[0];

	enumerate_leds();

	return 0;
}
