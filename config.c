#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define CONFIG_FILE "/etc/touchbard/config.txt"

int read_mode_from_config(char *mode, size_t size) {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (file == NULL) {
        perror("Error opening configuration file");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "mode = ", 7) == 0) {
            strncpy(mode, line + 7, size - 1);
            mode[size - 1] = '\0';
            // Remove newline character if present
            char *newline = strchr(mode, '\n');
            if (newline) {
                *newline = '\0';
            }
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    fprintf(stderr, "Mode not found in configuration file\n");
    return -1;
}

