#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "threadpool.h"

#define THREAD 4
#define SIZE   8192

void *pool;
int left;
pthread_mutex_t lock;

int error;

void dummy_task(void *arg)
{
    usleep(100);
    pthread_mutex_lock(&lock);
    left--;
    pthread_mutex_unlock(&lock);
}

int main(int argc, char **argv)
{
    int i;

    pthread_mutex_init(&lock, NULL);

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

    pthread_mutex_destroy(&lock);

    return 0;
}
