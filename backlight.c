#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <linux/input.h>
#include <dirent.h>
#include "backlight.h"
#include "device.h"

#define TIMEOUT 30
#define BACKLIGHT "/sys/class/backlight/appletb_backlight/brightness"

void write_backlight(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s", content);
    fclose(file);
}

char read_backlight(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return '\0';
    }
    char content = fgetc(file);
    fclose(file);
    return content;
}

void backlight() {

    char keyboard_device[256];
    if (find_device(KEYBOARD_DEV, keyboard_device) != 0) {
        exit(EXIT_FAILURE);
    }

    char trackpad_device[256];
    if (find_device(TRACKPAD_DEV, trackpad_device) != 0) {
        exit(EXIT_FAILURE);
    }

    char touchbar_device[256];
    if (find_device(TOUCHBAR_DEV, touchbar_device) != 0) {
        exit(EXIT_FAILURE);
    }

    int input_fd_keyboard = open(keyboard_device, O_RDONLY);
    if (input_fd_keyboard < 0) {
        perror("Error opening keyboard device");
        exit(EXIT_FAILURE);
    }

    int input_fd_trackpad = open(trackpad_device, O_RDONLY);
    if (input_fd_trackpad < 0) {
        perror("Error opening trackpad device");
        close(input_fd_keyboard);
        exit(EXIT_FAILURE);
    }

    int input_fd_touchbar = open(touchbar_device, O_RDONLY);
    if (input_fd_touchbar < 0) {
        perror("Error opening touchbar device");
        close(input_fd_keyboard);
        close(input_fd_trackpad);
        exit(EXIT_FAILURE);
    }

    struct timeval timeout;
    fd_set read_fds;
    struct input_event event;
    int activity;
    int max_fd = (((input_fd_keyboard > input_fd_trackpad) ? input_fd_keyboard : input_fd_trackpad) > input_fd_touchbar) ? ((input_fd_keyboard > input_fd_trackpad) ? input_fd_keyboard : input_fd_trackpad) : input_fd_touchbar;
    int timer_status = 1;

    while (1) {
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(input_fd_keyboard, &read_fds);
        FD_SET(input_fd_trackpad, &read_fds);
        FD_SET(input_fd_touchbar, &read_fds);

        activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0) {
            perror("select error");
            close(input_fd_keyboard);
            close(input_fd_trackpad);
            close(input_fd_touchbar);
            exit(EXIT_FAILURE);
        } else if (activity == 0) {
            if (timer_status == 1) {
                if (read_backlight(BACKLIGHT) != '1') {
                    write_backlight(BACKLIGHT, "1");
                }
                timer_status = 0;
            } else {
                if (read_backlight(BACKLIGHT) != '0') {
                    write_backlight(BACKLIGHT, "0");
                }
            }
        } else {
            if (FD_ISSET(input_fd_keyboard, &read_fds)) {
                read(input_fd_keyboard, &event, sizeof(struct input_event));
                write_backlight(BACKLIGHT, "2");
                timer_status = 1;
            } 
            if (FD_ISSET(input_fd_trackpad, &read_fds)) {
                read(input_fd_trackpad, &event, sizeof(struct input_event));
                write_backlight(BACKLIGHT, "2");
                timer_status = 1;
            }
            if (FD_ISSET(input_fd_touchbar, &read_fds)) {
                read(input_fd_touchbar, &event, sizeof(struct input_event));
                write_backlight(BACKLIGHT, "2");
                timer_status = 1;
            }
        }
    }

    close(input_fd_keyboard);
    close(input_fd_trackpad);
    close(input_fd_touchbar);
}

