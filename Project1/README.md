# CS3502 - Project 1: Multi-Threaded Banking System

## Overview
This project demonstrates multi-threaded programming concepts through the use of POXIS threads ('pthreads').
It is split up into four phases with each building upon each other. It begins with showcasing race conditions, introducing synchronization, deadlocks, and deadlock prevention.

-----

## Approaches to Each Phase

### **Phase 1**
For Phase 1, I implemented a basic multithreaded banking program with three threads performing deposits and withdrawals on a shared balance.  
This phase was designed to demonstrate how race conditions occur when multiple threads access shared data without synchronization.  
Since no locking was used, the final balance was different for each run, confirming that simultaneous writes caused inconsistent results.

---

### **Phase 2**
In Phase 2, I introduced mutex locks to prevent race conditions.  
Each account included its own `pthread_mutex_t` lock, allowing only one thread to modify that account at a time.  
Ten accounts were created with an initial balance of 1000, and four threads executed 50,000 random transactions each.  
After all threads completed their operations, I released the locks to free resources.  
This synchronization ensured consistent results and eliminated the inconsistencies seen in Phase 1.

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
To compile each phase, run the following command in your terminal:

```bash
gcc -pthread phaseX.c -o phaseX

./phaseX

time ./phaseX

