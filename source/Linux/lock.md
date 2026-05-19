# Multiple Threads

[TOC]

## Memory order

CPUs have complex cache hierarchies and can execute instructions out-of-order for performance. Compilers also reorder operations during optimization. Without proper synchronization, different threads might observe memory changes in different orders, breaking program logic.

- memory_order_relaxed: there are no synchronization or ordering constraints imposed on other reads or writes, only this operation's atomicity is guaranteed.

- memory_order_consume: A load operation with this memory order performs a consume operation on the affected memory location: no reads or writes in the current thread dependent on the value currently loaded can be reordered before this load. Writes to data - dependent variables in other threads that release the same atomic variable are visible in the current thread.On most platforms, this affects compiler optimizations only.

- memory_order_acquire: A load operation with this memory order performs the acquire operation on the affected memory location: no reads or writes in the current thread can be reordered before this load. All writes in other threads that release the same atomic variable are visible in the current thread.
            
- memory_order_release: A store operation with this memory order performs the release operation : no reads or writes in the current thread can be reordered after this store. All writes in the current thread are visible in other threads that acquire the same atomic variable and writes that carry a dependency into the atomic variable become visible in other threads that consume the same atomic.
                    
- memory_order_acq_rel: A read - modify - write operation with this memory order is both an acquire operation and a release operation.No memory reads or writes in the current thread can be reordered before the load, nor after the store All writes in other threads that release the same atomic variable are visible before the modification and the modification is visible in other threads that acquire the same atomic variable.
    
- memory_order_seq_cst: A load operation with this memory order performs an acquire operation, a store performs a release operation, and read - modify - write performs both an acquire operation and a release operation, plus a single total order exists in which all threads observe all modifications in the same order.

## FUTEX

### Introduction

A futex (short for fast userspace mutex) is a Linux kernel system call that provides a mechanism for fast locking and synchronization between threads in a program.The key idea is that the lock operations happen mostly in user space—only going into the kernel when absolutely necessary (such as when a thread needs to sleep waiting for a lock to be released). This reduces the overhead of frequent context switches and improves performance for multi-threaded applications.

It’s often used internally in threading libraries like pthread to implement synchronization primitives like mutexes and condition variables.

Under the hood, a futex usually involves a memory location in shared memory and a kernel call like futex(2) that can perform operations such as waiting for a value or waking waiting threads.

- FUTEX_WAIT
- FUTEX_WAKE

#### Wait operation

The FUTEX_WAIT operation is used to put a thread to sleep until a certain condition on a shared integer variable is met. Specifically, it tells the kernel: "Put me to sleep if the value in this memory location is equal to the expected value." If the value is different when you call it, the kernel will return immediately, avoiding unnecessary blocking.

Example call:

```c
syscall(SYS_futex, &futex_var, FUTEX_WAIT, expected_value, NULL, NULL, 0)
```

- &futex_var is the address we're synchronizing on
- expected_value is the integer we expect to see
- NULL means no timeout specified

#### Wake operation

Example call:

```c
syscall(SYS_futex, &futex_var, FUTEX_WAKE, 1, NULL, NULL, 0)
```

1 means wake up just one waiting thread.

Importantly, FUTEX_WAKE doesn't check the value in memory—it simply wakes threads waiting on that futex key. Coordination between value changes and wake calls must be enforced in user space using atomic writes.

## POSIX sync primitives

### Mutex

A mutex (mutual exclusion object) is a synchronization primitive used to protect shared resources from being accessed by multiple threads at the same time. A mutex provides a simple lock/unlock mechanism that ensures only one thread can enter a critical section.

It can be implemented in a variety of ways, but in POSIX systems, it usually builds on low-level synchronization mechanisms such as futexes on Linux. A simple conceptual implementation involves an atomic flag to indicate lock state and mechanisms to block and wake threads.

- lock

```c
    while (1) {

        if (__sync_bool_compare_and_swap(futexp, 1, 0))
            break;      /* Yes */

        s = futex(futexp, FUTEX_WAIT, 0, NULL, NULL, 0);
    }
```

- unlock

```c
    if (__sync_bool_compare_and_swap(futexp, 0, 1)) {
        s = futex(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
    }
```

### Condition variable

A condition variable (pthread_cond_t) is a synchronization primitive used to block a thread until a particular condition is met. Unlike a mutex, a condition variable provides a way for threads to wait and be signaled when a specific state change occurs.

```c
// Pseudo implementation of pthread_cond_wait
pthread_cond_wait(cond, mutex) {
    add_thread_to_wait_queue(cond);
    release_mutex(mutex);
    futex_wait(cond->counter);
    acquire_mutex(mutex);
}

pthread_cond_signal(cond) {
    if (!wait_queue_empty(cond)) {
        remove_one_thread(cond);
        futex_wake(cond->counter, 1);
    }
}

pthread_cond_broadcast(cond) {
    wake_all_threads_in_queue(cond);
    futex_wake(cond->counter, MAX);
}
```

### Semphore

```c
void my_sem_init(my_sem_t *sem, int value) {
    sem->count = value;
}

void my_sem_wait(my_sem_t *sem) {
    while (1) {
        int c = sem->count;
        if (c > 0 && __sync_bool_compare_and_swap(&sem->count, c, c - 1)) {
            return; // Acquired semaphore slot
        }
        futex_wait(&sem->count, c);
    }
}

void my_sem_post(my_sem_t *sem) {
    __sync_add_and_fetch(&sem->count, 1);
    futex_wake(&sem->count, 1);
}
```

## Atomic operation

### built-in

In GCC and Clang, __sync and __atomic are two sets of built-in functions provided for performing atomic operations, but they differ in capability, portability, and flexibility

- __sync
    - Only provide full sequential consistency memory ordering — you cannot specify weaker memory orderings like acquire, release, or relaxed. 
    - Functions include: __sync_fetch_and_add, __sync_fetch_and_sub, __sync_bool_compare_and_swap, etc.
    - These operations always act as a full memory barrier (both acquire and release), which is safe but can be slower than necessary.

- __atomic
    - Introduced to match the C++11 memory model.
    - Allow specifying the memory ordering precisely: __ATOMIC_RELAXED, __ATOMIC_ACQUIRE, __ATOMIC_RELEASE, __ATOMIC_ACQ_REL, __ATOMIC_SEQ_CST
    - Support finer control over performance vs. safety.
    - Functions include: __atomic_load_n, __atomic_store_n, __atomic_fetch_add, __atomic_compare_exchange_n, etc.

Prefer __atomic built-ins for new code, because they are more flexible and map directly to the C++ memory model.