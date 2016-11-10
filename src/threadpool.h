#ifndef __TPOOL_H__
#define __TPOOL_H__

enum schedule_type {
    ROUND_ROBIN,
    LEAST_LOAD
};

void *tpool_init(int num_worker_threads);

int tpool_inc_threads(void *pool, int num_inc);

void tpool_dec_threads(void *pool, int num_dec);

int tpool_add_work(void *pool, void(*routine)(void *), void *arg);
/*
@finish:  1, complete remaining works before return
        0, drop remaining works and return directly
*/
void tpool_destroy(void *pool, int finish);

/* set thread schedule algorithm, default is round-robin */
void set_thread_schedule_algorithm(void *pool, enum schedule_type type);

// Structures for MapReduce ////////////////////////////////////////////////////
int threadpool_map(void *pool, int size,
                   void (*routine)(int n, void *),
                   void *arg, int flags);
/**
* @struct threadpool_reduce_t
* @brief arguments    for threadpool_reduce
* @param reduce       perform reduce on 2 operands. save result in left
* @param reduce_alloc_neutral allocate object which is neutral for your reduce
* operation
* @param reduce_free  free object allocated with reduce_alloc_neutral.
* For every call to reduce_alloc_neutral coresponding reduce_free will also
* be called.
* @param object_size  size of objects to reduce on them. used to
*                     increment and decrement pointers.
* @param begin        first object for reduce
* @param end          object after the last
* @param self         user-specific data
*/
typedef struct {
    void (*reduce)(void *self, void *left, void *right);
    void *(*reduce_alloc_neutral)(void *self);
    void (*reduce_free)(void *self, void *node);
    void (*reduce_finish)(void *self, void *node);

    int object_size;
    void *begin;
    void *end;
    void *self;
} threadpool_reduce_t;

/**
* @function threadpool_reduce
* @brief parallel blocking reduce
* @param pool  Thread pool to which add the task.
* @param reduce  Filled threadpool_reduce_t struct.
* @return error code
*/
int threadpool_reduce(void *pool, threadpool_reduce_t *reduce);

/**
 * @function threadpool_destroy
 * @brief Stops and destroys a thread pool.
 * @param pool  Thread pool to destroy.
 * @param flags Flags for shutdown
 *
 * Known values for flags are 0 (default) and threadpool_graceful in
 * which case the thread pool doesn't accept any new tasks but
 * processes all pending tasks before shutdown.
 */
int threadpool_destroy(void *pool, int flags);

#endif
