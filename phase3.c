// Phase 3 - Showcases how inconsistent lock ordering can cause deadlock
#include <pthread.h>
#include <stdio.h>
#include <unistd.h> 

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

static Account accounts[2];   

// Locks are taken in opposite order to deliberately cause a deadlock
void transfer(int from_id, int to_id, double amount) {
    printf("Thread %lu: Attempting transfer from %d to %d\n",
           (unsigned long)pthread_self(), from_id, to_id);
	
    // Locks the first account
    pthread_mutex_lock(&accounts[from_id].lock);
    printf("Thread %lu: Locked account %d\n",
           (unsigned long)pthread_self(), from_id);

    usleep(1000); // Used to encourage overlap so we can guarantee a deadlock

    // Now attempts to lock the second account
    printf("Thread %lu: Waiting for account %d\n",
           (unsigned long)pthread_self(), to_id);
    pthread_mutex_lock(&accounts[to_id].lock);

    // Attempts to perform the transfer
    accounts[from_id].balance -= amount;
    accounts[to_id].balance   += amount;

    // Unlocks both accounts
    pthread_mutex_unlock(&accounts[to_id].lock);
    pthread_mutex_unlock(&accounts[from_id].lock);
}

// Thread functions that perform transfers in opposite directions
static void* t_AB(void* _){ (void)_; for(;;) transfer(0,1,1.0); return NULL; }
static void* t_BA(void* _){ (void)_; for(;;) transfer(1,0,1.0); return NULL; }

int main(void) {
    // Initializes both accounts and their mutex locks
    for (int i = 0; i < 2; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = 1000.0;
        accounts[i].transaction_count = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    pthread_t A, B;
    pthread_create(&A, NULL, t_AB, NULL);
    pthread_create(&B, NULL, t_BA, NULL);

    // Used to allow the threads to run for a few seconds so that the deadlock can be observed
    sleep(5);
    fprintf(stderr, "[WATCH] If the program appears stuck, deadlock occurred.\n");

    // Attempts to cancel/clean up threads; may not work if there's a deadlock
    pthread_cancel(A); pthread_cancel(B);
    pthread_join(A, NULL); pthread_join(B, NULL);

    for (int i = 0; i < 2; i++) pthread_mutex_destroy(&accounts[i].lock);
    return 0;
}
