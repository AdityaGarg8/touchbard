#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include "backlight.h"
#include "mode.h"
#include "config.h"

int main() {
    char mode[256];
    if (read_mode_from_config(mode, sizeof(mode)) != 0) {
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Child process: Handle mode switching
        handle_mode_switch(mode);
    } else {
        backlight();
    }
    return EXIT_SUCCESS;
}

