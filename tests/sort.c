#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "list.h"
#include "threadpool.h"

#ifdef PROFILE
#include "yastopwatch.h"

DEF_SW(map_time)
DEF_SW(reduce_time)
#endif

void my_map(int n, void *data)
{
    // empty
}

void my_reduce(void *self, void *left, void *right)
{
    llist_t *left_list, *right_list,
            *small, *remaining;
    node_t *current, *head;
    int size;

    left_list = *(llist_t **)left;
    right_list = *(llist_t **)right;

    head = current = NULL;
    size = 0;

    // for initialize head and current
    //   with non NULL pointer
    if (left_list->size && right_list->size) {
        small = (llist_t *) ( \
                              (left_list->head->data <= right_list->head->data) * (intptr_t) left_list + \
                              (left_list->head->data > right_list->head->data) * (intptr_t) right_list );

        head = current = small->head;

        small->head = small->head->next;
        small->size--;
        size++;

        current->next = NULL;
    }

    // merge two sorted list until either left or right become emtpy
    while (left_list->size && right_list->size) {
        small = (llist_t *) ( \
                              (left_list->head->data <= right_list->head->data) * (intptr_t) left_list + \
                              (left_list->head->data > right_list->head->data) * (intptr_t) right_list );

        current->next = small->head;
        current = current->next;

        small->head = small->head->next;
        small->size--;
        size++;

        current->next = NULL;
    }

    // append remaining list
    remaining = (llist_t *) ( \
                              (left_list->size > 0) * (intptr_t)left_list + \
                              (right_list->size > 0) * (intptr_t)right_list );
    if (remaining) {
        if (current) {
            current->next = remaining->head;
        } else {
            head = current = remaining->head;
        }
        size += remaining->size;
        remaining->head = NULL;
        remaining->size = 0;
    }

    // assign to left
    left_list->head = head;
    left_list->size = size;
}

void *my_alloc_neutral(void *self)
{
    llist_t **list;
    list = malloc(sizeof(llist_t *));
    *list = llist_create();
    return list;
}

void my_free(void *self, void *element)
{
    llist_t **list;
    list = (llist_t **) element;
    free(*list);
    free(list);
}

void my_finish(void *self, void *first_element)
{
    llist_t *result, **list;
    list = (llist_t **) first_element;
    result = (llist_t *) self;

    result->head = (*list)->head;
    result->size = (*list)->size;
}

int main(int argc, char *argv[])
{
    threadpool_t *pool;
    llist_t **data;
    llist_t *output_data;
    FILE *input_fptr;
    int i;

    if (argc != 4) {
        fprintf(stderr, "./sort [thread_counts] [queue_size] [data_size]\n");
        return -1;
    }

    const int threads = atoi(argv[1]);
    const int queue_size = atoi(argv[2]);
    const int data_size = atoi(argv[3]);

    data = malloc(data_size * sizeof(llist_t *));
    srand(time(NULL));
    for (i = 0; i < data_size; ++i) {
        data[i] = llist_create();
        llist_add(data[i], rand()%data_size+1);
    }

    output_data = llist_create();

    input_fptr = fopen("input.txt", "w");

    for (i = 0; i < data_size; ++i) {
        node_t *node = data[i]->head;
        while (node) {
            fprintf(input_fptr, "%d\n", node->data);
            node = node->next;
        }
    }

    fclose(input_fptr);

    pool = threadpool_create(threads, queue_size, 0);

#ifdef PROFILE
    START_SW(map_time);
#endif

    threadpool_map(pool, data_size, my_map, data, 0);

#ifdef PROFILE
    STOP_SW(map_time);
#endif

    threadpool_reduce_t reduce = {
        .begin = data,
        .end = data + data_size,
        .object_size = sizeof(llist_t *),
        .self = output_data,
        .reduce = my_reduce,
        .reduce_alloc_neutral = my_alloc_neutral,
        .reduce_finish = my_finish,
        .reduce_free = my_free,
    };

#ifdef PROFILE
    START_SW(reduce_time);
#endif

    threadpool_reduce(pool, &reduce);

#ifdef PROFILE
    STOP_SW(reduce_time);
#endif

    threadpool_destroy(pool, threadpool_graceful);

    for (i = 0; i < data_size; ++i) {
        // nodes have been moved to output_data
        llist_destroy(data[i]);
    }
    free(data);

    FILE *output_fptr;
    node_t *node;

    output_fptr = fopen("output.txt", "w");
    for (node = output_data->head; node ; node = node->next) {
        fprintf(output_fptr ,"%d\n", node->data);
    }

    fclose(output_fptr);

    llist_destroy(output_data);
#ifdef PROFILE
    fprintf(stderr, "[map] Total time: %lf\n", GET_SEC(map_time));
    fprintf(stderr, "[reduce] Total time: %lf\n", GET_SEC(reduce_time));
#endif

    return 0;
}
