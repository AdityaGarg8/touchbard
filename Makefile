CC = gcc
CFLAGS = -Wall
TARGET = touchbard

SRCS = main.c backlight.c mode.c config.c device.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) *.o
