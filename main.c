#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "backlight.h"
#include "mode.h"
#include "config.h"

void *mode_switch_thread(void *arg);
void *backlight_control_thread(void *arg);

int main() {
    char mode[256];
    if (read_mode_from_config(mode, sizeof(mode)) != 0) {
        return EXIT_FAILURE;
    }

    pthread_t mode_thread, backlight_thread;

    if (pthread_create(&mode_thread, NULL, mode_switch_thread, (void *)mode) != 0) {
        perror("Error creating mode switch thread");
        return EXIT_FAILURE;
    }

    if (pthread_create(&backlight_thread, NULL, backlight_control_thread, NULL) != 0) {
        perror("Error creating backlight thread");
        return EXIT_FAILURE;
    }

    pthread_join(mode_thread, NULL);
    pthread_join(backlight_thread, NULL);

    return EXIT_SUCCESS;
}

void *mode_switch_thread(void *arg) {
    char *mode = (char *)arg;
    handle_mode_switch(mode);
    return NULL;
}

void *backlight_control_thread(void *arg) {
    backlight();
    return NULL;
}
