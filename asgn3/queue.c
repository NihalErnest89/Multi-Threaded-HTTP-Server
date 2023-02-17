// Coded by Nihal Ernest

#include "queue.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>

/** @struct queue_t
 *
 *  @brief This typedef renames the struct queue.  Your `c` file
 *  should define the variables that you need for your queue.
 */
typedef struct queue {
    int len, count, in, out;
    void **buffer;

    pthread_mutex_t mutex;
    pthread_cond_t cv;

} queue;

/** @brief Dynamically allocates and initializes a new queue with a
 *         maximum size, size
 *
 *  @param size the maximum size of the queue
 *
 *  @return a pointer to a new queue_t
 */
queue_t *queue_new(int size) {
    queue_t *q = malloc(sizeof(queue));


    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cv, NULL);
   
    q->count = 0;
    q->in = 0;
    q->out = 0;

    q->len = size;
    q->buffer = (void **) malloc(size * sizeof(void *));
  

    return q;
}

/** @brief Delete your queue and free all of its memory.
 *
 *  @param q the queue to be deleted.  Note, you should assign the
 *  passed in pointer to NULL when returning (i.e., you should set
 *  *q = NULL after deallocation).
 *
 */
void queue_delete(queue_t **q) {
    if (q != NULL && *q != NULL) {
	free(*q);
	*q = NULL;	
    }

    pthread_mutex_destroy(&(*q)->mutex);
    pthread_cond_destroy(&(*q)->cv);
}

/** @brief push an element onto a queue
 *
 *  @param q the queue to push an element into.
 *
 *  @param elem th element to add to the queue
 *
 *  @return A bool indicating success or failure.  Note, the function
 *          should succeed unless the q parameter is NULL.
 */
bool queue_push(queue_t *q, void *elem) {
    // if the queue is null or full
    if (q == NULL || q->count >= q->len) {
        return 1;
    }

    pthread_mutex_lock(&q->mutex);

    while(q->count == q->len) {
       pthread_cond_wait(&q->cv, &q->mutex); 
    }

    q->buffer[q->in] = elem;
    q->in = (q->in + 1) % q->len;    

    q->count ++;

    pthread_mutex_unlock(&q->mutex);

    return 0;
}

/** @brief pop an element from a queue.
 *
 *  @param q the queue to pop an element from.
 *
 *  @param elem a place to assign the poped element.
 *
 *  @return A bool indicating success or failure.  Note, the function
 *          should succeed unless the q parameter is NULL.
 */
bool queue_pop(queue_t *q, void **elem) {
    // if the queue is null or empty
    if (q == NULL || q->count == 0) {
        return 1;
    }
     
    pthread_mutex_lock(&q->mutex);

    while (q->count == 0) {
        pthread_cond_wait(&q->cv, &q->mutex);
    }

    *elem = q->buffer[q->out];

    q->out = (q->out + 1) % q->len;
    q->count -= 1;


    return 0;
}
