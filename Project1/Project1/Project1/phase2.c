// Phase 2 - Mutexes added to prevent race conditions
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ACCOUNTS   10
#define INITIAL_BALANCE 1000.0
#define NUM_THREADS    4
#define TX_PER_THREAD  50000

// Account struct with per-account mutex
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;     
} Account;

static Account accounts[NUM_ACCOUNTS];

// Initializes all accounts and their mutex locks
static void init_accounts(void) {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

// Destroys all account locks to release resource after we're done using them
static void destroy_accounts(void) {
    for (int i = 0; i < NUM_ACCOUNTS; i++)
        pthread_mutex_destroy(&accounts[i].lock);
}

// Safely applies a deposit or withdrawal
static void apply_amount(int account_id, double amount) {
    if (account_id < 0 || account_id >= NUM_ACCOUNTS) return;
    pthread_mutex_lock(&accounts[account_id].lock);
    accounts[account_id].balance += amount;
    accounts[account_id].transaction_count++;
    pthread_mutex_unlock(&accounts[account_id].lock);
}

// Thread routine that performs random deposits/withdrawals
typedef struct { unsigned int seed; } ThreadArg;

// Generates random amount between -100 and +100
static inline double rand_amount(unsigned int *seed) {
    int r = (int)(rand_r(seed) % 201) - 100;  
    return (double)r;
}

static void* teller_thread(void *arg) {
    ThreadArg *a = (ThreadArg*)arg;
    for (int i = 0; i < TX_PER_THREAD; i++) {
        int acct = (int)(rand_r(&a->seed) % NUM_ACCOUNTS);
        double amt = rand_amount(&a->seed);
        apply_amount(acct, amt);
    }
    return NULL;
}

int main(void) {
    init_accounts();

    pthread_t th[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    
    // Launches 4 worker threads		
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].seed = (unsigned)time(NULL) ^ (0x9e3779b9u * (i+1));
        pthread_create(&th[i], NULL, teller_thread, &args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) pthread_join(th[i], NULL);

    double total = 0.0;
    int tx_sum = 0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        total += accounts[i].balance;
        tx_sum += accounts[i].transaction_count;
        printf("acct %d: balance=%.2f tx=%d\n",
               accounts[i].account_id, accounts[i].balance, accounts[i].transaction_count);
    }
    printf("accounts total: %.2f\n", total);
    printf("tx_sum: %d (expected %d)\n", tx_sum, NUM_THREADS * TX_PER_THREAD);

    destroy_accounts();
    return 0;
}
