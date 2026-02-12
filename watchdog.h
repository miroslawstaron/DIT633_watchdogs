/*
 * @file watchdog.h
 * @brief Simple watchdog timer interface
 *
 * This header file defines the interface for a simple watchdog timer.
 * The watchdog timer is used to detect and recover from system malfunctions.
 * It provides functions to "kick" the watchdog and check if it has expired.
 *
 * Author: Miroslaw Staron
 * Date: 2026
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdbool.h>

/* Feed (kick) the watchdog */
void watchdog_kick(void);

/* Check if watchdog has expired */
bool watchdog_expired(void);

int simple_watchdog(int argc, char *argv[]);

int multithreaded_watchdog(int argc, char *argv[]);

void check_parameters(int argc, char *argv[]);

#endif // WATCHDOG_H