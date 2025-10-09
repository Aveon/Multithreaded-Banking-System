// Phase 1 - Shows race conditions with unsynchronized threads
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#define ITERS 2000000   // used for number of updates per thread

// Struct for transaction arguments
typedef struct {
    int    id;
    double amount;
} TxArgs;

double balance = 1000.0;        // shared balance variable

// Thread function
void* tx_thread(void* p) {
    TxArgs* a = (TxArgs*)p;
    printf("Thread %d: %s %.2f\n",
           a->id,
           (a->amount >= 0 ? "Depositing" : "Withdrawing"),
           fabs(a->amount));


    for (int i = 0; i < ITERS; i++) {
        double old = balance;
        old += a->amount;
        balance = old;               // Race conditions will occur here
        if ((i & 0x3FF) == 0) sched_yield(); // encourages interleaving
    }
    return NULL;
}

int main(void) {
    printf("Initial balance : %.2f\n", balance);

    pthread_t t1, t2, t3;
    TxArgs a1 = {1, +100.0};
    TxArgs a2 = {2, +100.0};
    TxArgs a3 = {3, -50.0 };

    // Launch 3 threads that will perform simultaneous updates
    pthread_create(&t1, NULL, tx_thread, &a1);
    pthread_create(&t2, NULL, tx_thread, &a2);
    pthread_create(&t3, NULL, tx_thread, &a3);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("Final balance : %.2f\n", balance);
    return 0;
}
