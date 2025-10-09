// Phase 2 - Mutex added to protect shared account
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 3
#define TRANSACTIONS_PER_TELLER 1
#define NUM_ACCOUNTS 1
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void deposit(int account_id, double amount) {
    if (pthread_mutex_lock(&accounts[account_id].lock) != 0) {
        perror("Failed to acquire lock");
        return;
    }

    accounts[account_id].balance += amount;
    accounts[account_id].transaction_count++;

    pthread_mutex_unlock(&accounts[account_id].lock);
}

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
        deposit(0, amount);

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

    // Initialize account and mutex
    accounts[0].account_id = 0;
    accounts[0].balance = INITIAL_BALANCE;
    accounts[0].transaction_count = 0;
    pthread_mutex_init(&accounts[0].lock, NULL);

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

    pthread_mutex_destroy(&accounts[0].lock);
    return 0;
}
