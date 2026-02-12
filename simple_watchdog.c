#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <watchdog.h>

#define WATCHDOG_TIMEOUT 3   // seconds

time_t last_kick;            // last time the watchdog was fed

/* Feed (kick) the watchdog */
void watchdog_kick(void)
{
    last_kick = time(NULL);
}

/* Check if watchdog has expired */
bool watchdog_expired(void)
{
    time_t now = time(NULL);
    return (now - last_kick) > WATCHDOG_TIMEOUT;
}

/**
 * @brief Main entry point for the watchdog timer demonstration
 * @param argc Number of command-line arguments
 * @param argv Command-line arguments
 * @return 0 on successful completion, 1 if watchdog timeout occurs
 * 
 * This function demonstrates the basic operation of a watchdog timer
 * by periodically kicking the watchdog during normal operation and
 * intentionally not kicking it to trigger a timeout condition.
 */
int simple_watchdog (int argc, char *argv[]) {
    // Check and display any command-line parameters
    check_parameters(argc, argv);

    printf("Starting program with watchdog...\n");

    // Initialize the watchdog timer with an initial kick
    watchdog_kick();

    // Main loop that runs for 10 iterations
    // First 5 iterations: normal operation with watchdog kicks
    // Last 5 iterations: simulate a bug by not kicking the watchdog
    for (int i = 0; i < 10; i++)
    {
        printf("Main loop iteration %d\n", i);

        // Check if we're in the normal operation phase (first half of iterations)
        if (i < 5)
        {
            // Normal operation: watchdog is kicked regularly to prevent timeout
            watchdog_kick();
            printf("Watchdog kicked\n");
        }
        else
        {
            // Simulate a bug or unresponsive system: watchdog NOT kicked
            // This will eventually trigger a watchdog timeout
            printf("Oops... watchdog not kicked!\n");
        }

        // Simulate work being done by sleeping for 1 second
        sleep(1);

        // Check if the watchdog timer has expired
        if (watchdog_expired())
        {
            // Watchdog timeout detected - system reset simulation
            printf("WATCHDOG TIMEOUT! System reset!\n");
            return 1; // Return error code to indicate reset
        }
    }

    // Normal program termination if watchdog never expires
    return 0;
}