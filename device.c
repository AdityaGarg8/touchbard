#include <glob.h>
#include <stdio.h>
#include <string.h>

int find_device(const char *device_event, char *path) {
    glob_t glob_result;
    int ret;

    ret = glob(device_event, GLOB_NOSORT, NULL, &glob_result);
    if (ret != 0 || glob_result.gl_pathc == 0) {
        fprintf(stderr, "No device found for pattern: %s\n", device_event);
        return -1;
    }

    strncpy(path, glob_result.gl_pathv[0], 255);
    path[255] = '\0';

    globfree(&glob_result);
    return 0;
}
