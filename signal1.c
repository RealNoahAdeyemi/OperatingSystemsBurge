/* signal1.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
// Per Chat GPT AI
volatile sig_atomic_t signal_received = 0; // global flag

void handler(int signum)
{
  printf("Hello World!\n");
  signal_received = 1; // set flag when signal received
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, handler); // register handler
  alarm(5); // schedule SIGALRM after 5 seconds

  while (!signal_received); // wait until handler sets flag

  printf("Turing was right!\n");

  return 0;
}
