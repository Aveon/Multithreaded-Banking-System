// Phase 4 - Resolves the deadlock using consistent lock ordering
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// Account structure representing shared resources
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

static Account accounts[2];  

// Safe transfer using lock ordering to prevent circular waits
void safe_transfer(int from_id, int to_id, double amount) {
    
    // Locks the lower account ID first to avoid a deadlock
    int first  = (from_id < to_id) ? from_id : to_id;
    int second = (from_id < to_id) ? to_id   : from_id;

    pthread_mutex_lock(&accounts[first].lock);
    pthread_mutex_lock(&accounts[second].lock);

    accounts[from_id].balance -= amount;
    accounts[to_id].balance   += amount;

	
    // Unlocks each account in reverse order
    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);
}

static void* t_AB(void* _){ (void)_; for (int i=0;i<200000;i++) safe_transfer(0,1,1.0); return NULL; }
static void* t_BA(void* _){ (void)_; for (int i=0;i<200000;i++) safe_transfer(1,0,1.0); return NULL; }

int main(void) {
    for (int i = 0; i < 2; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = 1000.0;
        accounts[i].transaction_count = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    pthread_t A, B;
    pthread_create(&A, NULL, t_AB, NULL);
    pthread_create(&B, NULL, t_BA, NULL);

    pthread_join(A, NULL);
    pthread_join(B, NULL);

    printf("[ORDERED] Completed transfers without deadlock.\n");
    printf("Final balances: A0=%.2f A1=%.2f (sum=%.2f)\n",
           accounts[0].balance, accounts[1].balance,
           accounts[0].balance + accounts[1].balance);

    for (int i = 0; i < 2; i++) pthread_mutex_destroy(&accounts[i].lock);
    return 0;
}
