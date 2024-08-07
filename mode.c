#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <glob.h>
#include <string.h>
#include "mode.h"
#include "device.h"

#define KEY_FN 0x1d0
#define BASE_DIR "/sys/bus/hid/drivers/hid-appletb-kbd/*05AC:8302*"
#define FILE_NAME "mode"

void write_mode(const char *path, int value) {
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, FILE_NAME);

    FILE *file = fopen(full_path, "w");
    if (file == NULL) {
        perror("Error opening mode file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d\n", value);
    fclose(file);
}

int find_touchbar(char *path) {
    glob_t glob_result;
    int ret;

    ret = glob(BASE_DIR, GLOB_ONLYDIR | GLOB_NOSORT, NULL, &glob_result);
    if (ret != 0 || glob_result.gl_pathc == 0) {
        fprintf(stderr, "No matching directories found\n");
        return -1;
    }

    strncpy(path, glob_result.gl_pathv[0], 255);
    path[255] = '\0';

    globfree(&glob_result);
    return 0;
}

void handle_mode_switch(const char *mode) {
    char keyboard_device[256];
    if (find_device(KEYBOARD_DEV, keyboard_device) != 0) {
        exit(EXIT_FAILURE);
    }

    int default_value;
    int fn_value;

    if (!strcmp(mode, "function")) {
        default_value = 1;
        fn_value = 2;
    } else if (!strcmp(mode, "media")) {
        default_value = 2;
        fn_value = 1;
    } else {
        fprintf(stderr, "Unknown mode: %s\n", mode);
        exit(EXIT_FAILURE);
    }

    struct input_event ev;
    int fd = open(keyboard_device, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("Error opening device");
        exit(EXIT_FAILURE);
    }

    char touchbar[256];
    if (find_touchbar(touchbar) != 0) {
        close(fd);
        exit(EXIT_FAILURE);
    }

    write_mode(touchbar, default_value);

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    while (1) {
        if (select(fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select error");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(fd, &read_fds)) {
            if (read(fd, &ev, sizeof(struct input_event)) == -1) {
                perror("Error reading input event");
                close(fd);
                exit(EXIT_FAILURE);
            }

            if (ev.type == EV_KEY && ev.code == KEY_FN) {
                if (ev.value == 1) {
                    write_mode(touchbar, fn_value);
                } else if (ev.value == 0) {
                    write_mode(touchbar, default_value);
                }
            }
        }
    }

    close(fd);
}

