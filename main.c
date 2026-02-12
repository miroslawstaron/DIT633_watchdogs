/**
 * @file main.c
 * @brief Main program integrating all parts from Lecture 1
 *
 * This program integrates all parts from Lecture 5
 * It presents the concept of a simple watchdog timer, 
 * which is a common component in embedded systems to ensure that 
 * the system is operating correctly. 
 * The watchdog timer is designed to reset the system 
 * if it detects that the system has become unresponsive or 
 * is not functioning as expected. 
 * 
 * Author: Miroslaw Staron
 * Date: 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <watchdog.h>

/**
 * @brief Checks and prints any command-line parameters passed to the program
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 */
void check_parameters(int argc, char *argv[]) {
    if (argc > 1) {
        printf("Parameters received:\n");
        for (int i = 1; i < argc; i++) {
            printf("  argv[%d] = %s\n", i, argv[i]);
        }
    } else {
        printf("No parameters provided.\n");
    }
}

int main(int argc, char *argv[])
{
    int result;

    check_parameters(argc, argv);

    
    printf("=== Simple Watchdog Timer Demo ===\n");
    result = simple_watchdog(argc, argv);
    if (result == 0) {
        printf("Program completed successfully without watchdog timeout.\n");
    } else {
        printf("Program terminated due to watchdog timeout.\n");
    }
    return result;
    
    /*
    printf("\n=== Multi-threaded Watchdog Timer Demo ===\n");
 
    result = multithreaded_watchdog(argc, argv);
    if (result == 0) {
        printf("Program completed successfully without watchdog timeout.\n");
    } else {
        printf("Program terminated due to watchdog timeout.\n");
    }
    return result;
    */
}