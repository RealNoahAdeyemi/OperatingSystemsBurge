#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

#define LOOPS 25

// Generaated with You.com AI

// Shared semaphore name
#define SEM_NAME "/bankaccount_sem"

void cleanup(sem_t *sem, int *BankAccount) {
    sem_close(sem);
    sem_unlink(SEM_NAME);
    munmap(BankAccount, sizeof(int));
}

int main() {
    // Shared memory for BankAccount
    int *BankAccount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *BankAccount = 0;

    // Create a named semaphore for mutual exclusion, initial value 1
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    srand(time(NULL) ^ getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) {
        // Parent process: Dear Old Dad
        for (int i = 0; i < LOOPS; i++) {
            sleep(rand() % 6); // Sleep 0-5 seconds

            printf("Dear Old Dad: Attempting to Check Balance\n");

            sem_wait(sem); // Enter critical section

            int localBalance = *BankAccount;
            int r = rand();
            if (r % 2 == 0) {
                if (localBalance < 100) {
                    int amount = rand() % 101; // 0-100
                    if (amount % 2 == 0) {
                        localBalance += amount;
                        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
                        *BankAccount = localBalance;
                    } else {
                        printf("Dear old Dad: Doesn't have any money to give\n");
                    }
                } else {
                    printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
                }
            } else {
                printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
            }

            sem_post(sem); // Exit critical section
        }
        wait(NULL); // Wait for child to finish
        cleanup(sem, BankAccount);
    } else {
        // Child process: Poor Student
        for (int i = 0; i < LOOPS; i++) {
            sleep(rand() % 6); // Sleep 0-5 seconds

            printf("Poor Student: Attempting to Check Balance\n");

            sem_wait(sem); // Enter critical section

            int localBalance = *BankAccount;
            int r = rand();
            if (r % 2 == 0) {
                int need = rand() % 51; // 0-50
                printf("Poor Student needs $%d\n", need);
                if (need <= localBalance) {
                    localBalance -= need;
                    printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
                    *BankAccount = localBalance;
                } else {
                    printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
                }
            } else {
                printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
            }

            sem_post(sem); // Exit critical section
        }
        cleanup(sem, BankAccount);
        exit(0);
    }
    return 0;
}
