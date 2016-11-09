#define THREAD 32
#define QUEUE  256

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "threadpool.h"

int tasks = 0, done = 0;
pthread_mutex_t lock;

void dummy_task(void *arg) {
    usleep(10000);
    pthread_mutex_lock(&lock);
    done++;
    pthread_mutex_unlock(&lock);
}

int main(int argc, char **argv)
{
    threadpool_t *pool;
    int tmp_done;

    pthread_mutex_init(&lock, NULL);

    assert((pool = threadpool_create(THREAD, QUEUE, 0)) != NULL);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);

    while(threadpool_add(pool, &dummy_task, NULL, 0) == 0) {
        tasks++;
    }

    fprintf(stderr, "Added %d tasks\n", tasks);

    pthread_mutex_lock(&lock);
    tmp_done = done;
    pthread_mutex_unlock(&lock);

    while((tasks / 2) > tmp_done) {
        usleep(10000);
        pthread_mutex_lock(&lock);
        tmp_done = done;
        pthread_mutex_unlock(&lock);
    }
    assert(threadpool_destroy(pool, 0) == 0);
    fprintf(stderr, "Did %d tasks\n", done);

    return 0;
}
