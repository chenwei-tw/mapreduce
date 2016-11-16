#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdatomic.h>

#include "threadpool.h"

#define THREAD 4
#define SIZE   8192
#define QUEUES 4

/*
 * Warning do not increase THREAD and QUEUES too much on 32-bit
 * platforms: because of each thread (and there will be THREAD *
 * QUEUES of them) will allocate its own stack (8 MB is the default on
 * Linux), you'll quickly run out of virtual space.
 */

void *pool[QUEUES];
int tasks[SIZE], left;

int error;

void dummy_task(void *arg)
{
    int *pi = (int *)arg;
    *pi += 1;

    if(*pi < QUEUES) {
        assert(tpool_add_work(pool[*pi], &dummy_task, arg) == 0);
    } else {
        atomic_fetch_sub(&left, 1);
    }
}

int main(int argc, char **argv)
{
    int i, copy = 1;

    left = SIZE;

    for(i = 0; i < QUEUES; i++) {
        pool[i] = tpool_init(THREAD);
        assert(pool[i] != NULL);
    }

    usleep(10);

    for(i = 0; i < SIZE; i++) {
        tasks[i] = 0;
        assert(tpool_add_work(pool[0], &dummy_task, &(tasks[i])) == 0);
    }

    while(copy > 0) {
        usleep(1);
        atomic_store(&copy, left);
    }

    for(i = 0; i < QUEUES; i++) {
        tpool_destroy(pool[i], 0);
    }

    return 0;
}
