#define THREAD 8
#define QUEUE  256
#define MAP_TASK_NUM 16

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"
#include "yastopwatch.h"

void is_simple(int n, void *_data)
{
    int *data = (int *) _data;
    int x = data[n];
    data[n] = 0;
    if (x < 2) return;
    for (int i = 2; i < x; i++)
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
    FILE *output = fopen("opt.txt", "w");
    
    DEF_SW(total_time); 
    //DEF_SW(map_time); 
    //DEF_SW(reduce_time); 

    RESET_SW(total_time);
    //RESET_SW(map_time);
    //RESET_SW(reduce_time);

    threadpool_t *pool;
    
    int datasize = 0;
    double time_sum = 0, time_avg = 0;
    int count = 0;
    
    for(datasize = 20000; datasize < 100000; datasize += 800) {
        int *data = malloc(datasize * sizeof(int));    
        printf("completed percentage: %d %\n", count++);
        for(int turn = 0; turn < 30; turn++) {
            system("echo 3 | sudo tee /proc/sys/vm/drop_caches");
            START_SW(total_time);
            pool = threadpool_create(THREAD, QUEUE, 0);
            //fprintf(stderr, "Pool started with %d threads and queue size of %d\n", THREAD, QUEUE);
            for (int i = 0; i < datasize; i++)
                data[i] = i + 1;

            //START_SW(map_time);
            threadpool_map(pool, datasize, MAP_TASK_NUM, is_simple, data, 0);
            //STOP_SW(map_time);
            
            //for (int i = 0; i < DATASIZE; i++)
            //printf("%c", !!data[i] ? '-' : ' ');
            //printf("\n");

            threadpool_reduce_t reduce = {
                .begin = data,
                .end = data + datasize,
                .object_size = sizeof(*data),
                .self = NULL,
                .reduce = my_reduce,
                .reduce_alloc_neutral = my_alloc_neutral,
                .reduce_finish = my_finish,
                .reduce_free = my_free,
            };

	        //START_SW(reduce_time);
	        threadpool_reduce(pool, &reduce);
	        //STOP_SW(reduce_time);
	
            STOP_SW(total_time);

	        //fprintf(stderr, "[map] Total time: %lf sec\n", GET_SEC(map_time));
	        //fprintf(stderr, "[reduce] Total time: %lf sec\n", GET_SEC(reduce_time));
	        //fprintf(stderr, "[total] Total time: %lf sec\n", GET_SEC(total_time));
            
            time_sum += GET_SEC(total_time);

            RESET_SW(total_time);
            //RESET_SW(map_time);
            //RESET_SW(reduce_time);
            threadpool_destroy(pool, 0);
        }
        time_avg = time_sum / 30.0;
        fprintf(output,"%d %lf\n", datasize, time_avg);
        free(data);
        time_sum = 0;
        time_avg = 0;
    }
    fclose(output);
	return 0;
}
