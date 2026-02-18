Multi-Threaded Banking System

## Overview
This project demonstrates multi-threaded programming concepts through the use of POSIX threads ('pthreads').
It is split up into four phases with each building upon each other. It begins with showcasing race conditions, introducing synchronization, deadlocks, and deadlock prevention.

-----

## Approaches to Each Phase

### **Phase 1**
For Phase 1, I implemented a basic multithreaded banking program with three threads performing deposits and withdrawals on a shared balance.  
This phase was designed to demonstrate how race conditions occur when multiple threads access shared data without synchronization.  
Since no locking was used, the final balance was different for each run, confirming that simultaneous writes caused inconsistent results.

---

### **Phase 2**
In Phase 2, I introduced a mutex lock to protect the common balance against race conditions.
One of the account structures was changed to have a `pthread_mutex_t` mutex lock that allows for a deposit or withdrawal by a single thread at a time.
Three threads were utilized: two depositing $100 each and one withdrawing $50.
Unlike Phase 1, the program provided an identical and correct end balance on each run, ensuring that synchronization precluded interference among threads.


---

### **Phase 3**
For Phase 3, I demonstrated how deadlocks can occur when threads lock shared resources in different orders.  
I used two accounts and two threads performing transfers in opposite directions.  
A small `usleep()` delay was inserted between locking actions to increase overlap and make a deadlock more likely.  

---

### **Phase 4**
In Phase 4, I resolved the deadlock issue by implementing a consistent lock ordering rule.  
The program compared account IDs and always locked the lower one first, followed by the higher one.  
This prevented circular waits and completely removed the deadlock condition.  

-----

## 🛠️ How to Compile and Run

### **1. Compile Each Phase**
To compile each phase, run the following command in your terminal(X = number of phase):

```bash
gcc -pthread phaseX.c -o phaseX
```
### 2. Run the Programs

To execute the program, use(X = number of phase):

```bash
./phaseX
```
Performance can be measured using the built-in time command(X = number of phase):

```bash
time ./phaseX
```



