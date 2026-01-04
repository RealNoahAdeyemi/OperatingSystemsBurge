/* timer.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
// Per Chat GPT AI
volatile sig_atomic_t alarm_count = 0;  // number of SIGALRM received
time_t start_time;                      // time when program started

void alarm_handler(int signum)
{
    printf("Hello World!\n");
    alarm_count++;
    printf("Turing was right!\n");
    alarm(1); // schedule the next alarm in 1 second
}

void int_handler(int signum)
{
    time_t end_time = time(NULL);
    double total_time = difftime(end_time, start_time);

    printf("\nProgram executed for %.0f seconds.\n", total_time);
    printf("Total alarms (seconds elapsed): %d\n", alarm_count);

    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGALRM, alarm_handler); // handle SIGALRM
    signal(SIGINT, int_handler);    // handle SIGINT (Ctrl-C)

    start_time = time(NULL);        // record start time
    alarm(1);                       // schedule first alarm

    while (1)
        pause(); // sleep until a signal is received

    return 0;
}
