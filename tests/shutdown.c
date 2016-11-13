#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdatomic.h>

#include "threadpool.h"

#define THREAD 4
#define SIZE   8192

void *pool;
int left;

int error;

void dummy_task(void *arg)
{
    usleep(100);
    atomic_fetch_sub(&left, 1);
}

int main(int argc, char **argv)
{
    int i;

    /* Testing immediate shutdown */
    left = SIZE;
    pool = tpool_init(THREAD);
    for(i = 0; i < SIZE; i++) {
        assert(tpool_add_work(pool, &dummy_task, NULL) == 0);
    }
    tpool_destroy(pool, 0);
    assert(left > 0);

    /* Testing graceful shutdown */
    left = SIZE;
    pool = tpool_init(THREAD);
    for(i = 0; i < SIZE; i++) {
        assert(tpool_add_work(pool, &dummy_task, NULL) == 0);
    }
    tpool_destroy(pool, 1);
    assert(left == 0);

    return 0;
}
