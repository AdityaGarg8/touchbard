#ifndef DEVICE_H
#define DEVICE_H

int find_device(const char *device_event, char *path);

#endif 

#ifndef KEYBOARD_DEV
#define KEYBOARD_DEV "/dev/input/by-id/*Apple_Internal_Keyboard*event-kbd"
#endif

#ifndef TRACKPAD_DEV
#define TRACKPAD_DEV "/dev/input/by-id/*Apple_Internal_Keyboard*event-mouse"
#endif

#ifndef TOUCHBAR_DEV
#define TOUCHBAR_DEV "/dev/input/by-id/*Apple_Inc._Touch_Bar_Display*"
#endif
