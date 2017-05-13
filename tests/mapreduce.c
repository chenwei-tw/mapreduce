#define THREAD 8
#define QUEUE  256
#define MAP_TASK_NUM 16
#define FROM 20000
#define TO 100000
#define DELTA 800
#define SAMPLES 30

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "threadpool.h"
#include "yastopwatch.h"

void is_simple(int n, void *_data)
{
    int *data = (int *) _data;
    int x = data[n];
    data[n] = 0;
    if (x < 2) return;
    for (int i = 2; i <= x; i++)
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
    //printf("reduce result = %d\n", *(int *) node);
}

int main(int argc, char *argv[])
{
    DEF_SW(total_time); 
    RESET_SW(total_time);

    FILE *fout = fopen("opt3.txt", "w");
    double time_sum = 0, time_avg = 0;
    int count = 0;
    threadpool_t *pool;

    for (int DATASIZE = FROM; DATASIZE < TO; DATASIZE += DELTA) {
        int *data = malloc(DATASIZE * sizeof(int));
        printf("completed percentage: %d %\n", count++);
        for (int j = 0; j < SAMPLES; j++) {
            system("echo 3 | sudo tee /proc/sys/vm/drop_caches");
            START_SW(total_time);
            pool = threadpool_create(THREAD, QUEUE, 0);

            for (int i = 0; i < DATASIZE; i++)
                data[i] = i + 1;

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

            mapreduce(pool, DATASIZE, MAP_TASK_NUM, is_simple, data, 0, &reduce);
            STOP_SW(total_time);
            time_sum += GET_SEC(total_time);
            RESET_SW(total_time);
            threadpool_destroy(pool, 0);
        }
        time_avg = time_sum / SAMPLES;
        fprintf(fout, "%d %lf\n", DATASIZE, time_avg);
        free(data);
        time_sum = 0;
        time_avg = 0;
    }

    fclose(fout);
    return 0;
}
