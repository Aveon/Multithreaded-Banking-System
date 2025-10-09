// Phase 1 - Shows race conditions with unsynchronized threads
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 3
#define TRANSACTIONS_PER_TELLER 1
#define NUM_ACCOUNTS 1

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
} Account;

Account accounts[NUM_ACCOUNTS];

void* teller_thread(void* arg) {
    int teller_id = *(int*)arg;

    double amount;
    switch (teller_id) {
        case 1: amount = 100.0; break;
        case 2: amount = 100.0; break;
        case 3: amount = -50.0; break;
        default: amount = 0.0;
    }

    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        double before = accounts[0].balance;
        usleep(0);  // Encourage context switch
        accounts[0].balance = before + amount;
        accounts[0].transaction_count++;

        printf("Thread %d: %s %.2f\n",
               teller_id,
               (amount >= 0 ? "Depositing" : "Withdrawing"),
               fabs(amount));
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Initialize account
    accounts[0].account_id = 0;
    accounts[0].balance = 1000.00;
    accounts[0].transaction_count = 0;

    printf("Initial balance: %.2f\n", accounts[0].balance);

    // Start timer
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Final balance: %.2f\n", accounts[0].balance);
    printf("Execution time: %.6f seconds\n", elapsed);

    return 0;
}
