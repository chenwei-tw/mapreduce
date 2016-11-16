#ifndef LIST_H
#define LIST_H

typedef struct node {
    struct node *next;
    int data;
} node_t ;

typedef struct llist {
    node_t *head;
    int size;
} llist_t ;

llist_t *llist_create();
int llist_add(llist_t *list, int data);
int llist_pop_front(llist_t *list);
int llist_size(llist_t *list);

void llist_print(llist_t *list);
void llist_destroy(llist_t *list);

#endif
