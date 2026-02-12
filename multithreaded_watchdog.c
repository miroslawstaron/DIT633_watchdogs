/**
 * @file main_threads.c
 * @brief Multi-threaded watchdog timer demonstration
 *
 * This program demonstrates the use of a watchdog timer in a
 * multi-threaded environment. Each worker thread must periodically
 * "kick" its own watchdog entry. A dedicated monitor thread checks
 * all registered watchdogs and detects when any thread becomes
 * unresponsive.
 *
 * Scenario:
 *   - 3 worker threads perform periodic work and kick the watchdog.
 *   - After a few iterations, Thread 2 simulates a hang (stops kicking).
 *   - The watchdog monitor detects the timeout and signals all threads
 *     to shut down (simulating a system reset).
 *
 * Author: Miroslaw Staron
 * Date: 2026
 */

#define _DEFAULT_SOURCE   /* expose usleep() on glibc */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <watchdog.h>

/* ------------------------------------------------------------------ */
/*  Per-thread watchdog infrastructure                                 */
/* ------------------------------------------------------------------ */

#define NUM_WORKERS       3      /* number of worker threads            */
#define WATCHDOG_TIMEOUT  3      /* seconds before a thread is "dead"   */
#define MONITOR_INTERVAL  1      /* how often the monitor checks (sec)  */
#define WORKER_ITERATIONS 12     /* total iterations per worker thread  */

/* Each worker thread gets its own watchdog slot. */
typedef struct {
    pthread_t   tid;                /* thread id                       */
    int         id;                 /* logical thread number (0-based) */
    time_t      last_kick;          /* last time this thread kicked    */
    bool        alive;              /* false after watchdog expires     */
    const char *name;               /* human-readable name             */
} thread_watchdog_t;

/* Shared state ---------------------------------------------------- */
static thread_watchdog_t watchdogs[NUM_WORKERS];
static pthread_mutex_t   wd_mutex = PTHREAD_MUTEX_INITIALIZER;
static volatile bool     system_running = true;   /* global run flag  */

/* ------------------------------------------------------------------ */
/*  Watchdog helper functions (thread-safe)                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Kick (feed) the watchdog for the calling thread.
 * @param id  Logical thread id (index into watchdogs[])
 */
static void thread_watchdog_kick(int id)
{
    pthread_mutex_lock(&wd_mutex);
    watchdogs[id].last_kick = time(NULL);
    pthread_mutex_unlock(&wd_mutex);
}

/**
 * @brief Check whether a specific thread's watchdog has expired.
 * @param id  Logical thread id
 * @return true if the thread missed its deadline
 */
static bool thread_watchdog_expired(int id)
{
    bool expired;
    pthread_mutex_lock(&wd_mutex);
    expired = (time(NULL) - watchdogs[id].last_kick) > WATCHDOG_TIMEOUT;
    pthread_mutex_unlock(&wd_mutex);
    return expired;
}

/* ------------------------------------------------------------------ */
/*  Worker thread function                                             */
/* ------------------------------------------------------------------ */

/**
 * @brief Worker thread entry point.
 *
 * Each worker performs WORKER_ITERATIONS iterations of "work",
 * kicking the watchdog each time.  Thread 2 (id == 1) deliberately
 * stops kicking after iteration 5 to simulate a hang / bug.
 *
 * @param arg  Pointer to the thread's watchdog slot (thread_watchdog_t*)
 */
static void *worker_thread(void *arg)
{
    thread_watchdog_t *wd = (thread_watchdog_t *)arg;
    int id = wd->id;

    printf("[%s] Started.\n", wd->name);

    /* Initial kick so the monitor sees us immediately. */
    thread_watchdog_kick(id);

    for (int i = 0; i < WORKER_ITERATIONS && system_running; i++) {

        /* ---- Simulate bug in Thread 2 after iteration 5 ---------- */
        if (id == 1 && i >= 5) {
            printf("[%s] Iteration %2d — BUG: not kicking watchdog!\n",
                   wd->name, i);
        } else {
            /* Normal operation: do work and kick the watchdog */
            printf("[%s] Iteration %2d — working normally, kicking watchdog.\n",
                   wd->name, i);
            thread_watchdog_kick(id);
        }

        /* Simulate work – each thread sleeps for a different period
           so the output is not perfectly synchronized.              */
        usleep(800000 + (id * 200000));   /* 0.8 s – 1.2 s */
    }

    printf("[%s] Finished.\n", wd->name);
    return NULL;
}

/* ------------------------------------------------------------------ */
/*  Watchdog monitor thread                                            */
/* ------------------------------------------------------------------ */

/**
 * @brief Monitor thread that periodically checks every worker's watchdog.
 *
 * If any worker's watchdog expires, the monitor prints a timeout
 * message and sets the global run flag to false, causing all worker
 * threads to terminate (simulating a system reset).
 */
static void *monitor_thread(void *arg)
{
    (void)arg;   /* unused */

    printf("[Monitor] Watchdog monitor started (timeout = %d s).\n",
           WATCHDOG_TIMEOUT);

    while (system_running) {
        sleep(MONITOR_INTERVAL);

        for (int i = 0; i < NUM_WORKERS; i++) {
            if (!watchdogs[i].alive)
                continue;   /* already flagged */

            if (thread_watchdog_expired(i)) {
                printf("\n*** [Monitor] WATCHDOG TIMEOUT for %s! ***\n",
                       watchdogs[i].name);
                printf("*** [Monitor] Initiating system reset... ***\n\n");

                /* Mark the thread as dead and signal shutdown. */
                pthread_mutex_lock(&wd_mutex);
                watchdogs[i].alive = false;
                system_running = false;
                pthread_mutex_unlock(&wd_mutex);

                return NULL;   /* monitor exits */
            }
        }
    }

    printf("[Monitor] Watchdog monitor stopped.\n");
    return NULL;
}

/* ------------------------------------------------------------------ */
/*  Main                                                               */
/* ------------------------------------------------------------------ */



int multithreaded_watchdog(int argc, char *argv[])
{
    static const char *names[NUM_WORKERS] = {
        "Thread 1", "Thread 2", "Thread 3"
    };

    check_parameters(argc, argv);

    printf("======================================================\n");
    printf("  Multi-threaded Watchdog Timer Demonstration\n");
    printf("  Workers: %d   Timeout: %d s   Iterations: %d\n",
           NUM_WORKERS, WATCHDOG_TIMEOUT, WORKER_ITERATIONS);
    printf("  Thread 2 will stop kicking after iteration 5.\n");
    printf("======================================================\n\n");

    /* ---- Initialise per-thread watchdog slots -------------------- */
    time_t now = time(NULL);
    for (int i = 0; i < NUM_WORKERS; i++) {
        watchdogs[i].id        = i;
        watchdogs[i].last_kick = now;
        watchdogs[i].alive     = true;
        watchdogs[i].name      = names[i];
    }

    /* ---- Launch the watchdog monitor ----------------------------- */
    pthread_t monitor_tid;
    if (pthread_create(&monitor_tid, NULL, monitor_thread, NULL) != 0) {
        perror("pthread_create (monitor)");
        return EXIT_FAILURE;
    }

    /* ---- Launch worker threads ----------------------------------- */
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (pthread_create(&watchdogs[i].tid, NULL,
                           worker_thread, &watchdogs[i]) != 0) {
            perror("pthread_create (worker)");
            return EXIT_FAILURE;
        }
    }

    /* ---- Wait for everyone to finish ----------------------------- */
    for (int i = 0; i < NUM_WORKERS; i++)
        pthread_join(watchdogs[i].tid, NULL);

    pthread_join(monitor_tid, NULL);

    /* ---- Report result ------------------------------------------- */
    if (!system_running) {
        printf("\nProgram terminated due to watchdog timeout (system reset).\n");
        return EXIT_FAILURE;
    }

    printf("\nAll threads completed normally.\n");
    return EXIT_SUCCESS;
}
