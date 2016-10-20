#define THREAD 8
#define QUEUE  256
#define DATASIZE (20000)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "threadpool.h"

void is_simple(int n, void *_data)
{
    int *data = (int *) _data;
    int x = data[n];
    data[n] = 0;
    if (x < 2) return;
    if (x > 2 && x % 2 == 0) return;
    for (int i = 3; i * i <= x; i += 2)
        if (x % i == 0) return;
    data[n] = x;
}

void my_reduce(void *self, void *left, void *right)
{
    *((int *) left) += *((int *) right);
}

void *my_alloc_neutral(void *self)
{
    int *c = malloc(sizeof(int));
    *c = 0;
    return c;
}

void my_free(void *self, void *node)
{
    free(node);
}

void my_finish(void *self, void *node)
{
    printf("reduce result = %d\n", *(int *) node);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    struct timespec start, end;
    double cpu_time1, cpu_time2;
    threadpool_t *pool;

    pool = threadpool_create(THREAD, QUEUE, 0);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);

    int *data = malloc(DATASIZE * sizeof(int));
    for (int i = 0; i < DATASIZE; i++)
        data[i] = i + 1;

    clock_gettime(CLOCK_REALTIME, &start);

    threadpool_map(pool, DATASIZE, is_simple, data, 0);

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);
    fprintf(stderr, "map : %lf sec\n", cpu_time1);

    for (int i = 0; i < DATASIZE; i++)
        printf("%c", !!data[i] ? '-' : ' ');
    printf("\n");


    clock_gettime(CLOCK_REALTIME, &start);
    threadpool_reduce_t reduce = {
        .begin = data,
        .end = data + DATASIZE,
        .object_size = sizeof(*data),
        .self = NULL,
        .reduce = my_reduce,
        .reduce_alloc_neutral = my_alloc_neutral,
        .reduce_finish = my_finish,
        .reduce_free = my_free,
    };

    threadpool_reduce(pool, &reduce);

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);
    fprintf(stderr, "reduce : %lf sec\n", cpu_time2);

    return 0;
}
