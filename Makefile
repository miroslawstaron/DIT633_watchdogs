# Makefile for DIT633 Watchdog Timer Project

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -I.
TARGET  = lecture5
SRCS    = main.c simple_watchdog.c multithreaded_watchdog.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -pthread -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
