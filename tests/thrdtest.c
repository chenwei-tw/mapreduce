#define THREAD 32
#define QUEUE  256

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdatomic.h>

#include "threadpool.h"

int tasks = 0, done = 0;

void dummy_task(void *arg)
{
    usleep(10000);
    atomic_fetch_sub(&done, 1);
}

int main(int argc, char **argv)
{
    void *pool;
    int tmp_done;

    assert((pool = tpool_init(THREAD)) != NULL);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);

    while(tpool_add_work(pool, &dummy_task, NULL) == 0) {
        tasks++;
    }

    fprintf(stderr, "Added %d tasks\n", tasks);

    atomic_store(&tmp_done, done);

    while((tasks / 2) > tmp_done) {
        usleep(10000);
        atomic_store(&tmp_done, done);
    }
    tpool_destroy(pool, 0);
    fprintf(stderr, "Did %d tasks\n", done);

    return 0;
}
