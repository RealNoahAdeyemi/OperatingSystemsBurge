/* signal2.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
// Per Chat GPT AI
volatile sig_atomic_t signal_received = 0; // global flag

void handler(int signum)
{
  printf("Hello World!\n");
  signal_received = 1; // mark that the signal was handled
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, handler); // register the signal handler
  alarm(5); // schedule the first alarm

  while (1)
  {
    // wait until the signal handler sets the flag
    while (!signal_received)
      pause(); // sleep until a signal arrives

    // reset flag and perform main action
    signal_received = 0;
    printf("Turing was right!\n");

    // schedule the next alarm
    alarm(5);
  }

  return 0;
}
