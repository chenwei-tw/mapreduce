#include "list.h"
#include <stdlib.h>

llist_t *llist_create()
{
    llist_t *list;

    list = malloc(sizeof(llist_t));
    list->size = 0;
    list->head = NULL;

    return list;
}

int llist_add(llist_t *list, int data)
{
    node_t *node;

    if (list == NULL)
        return -1;

    node = malloc(sizeof(node_t));
    node->data = data;
    node->next = list->head;
    list->head = node;
    list->size++;

    return 0;
}

void llist_destroy(llist_t *list)
{
    node_t *now;

    if (list == NULL)
        return;

    while ((now = list->head)) {
        list->head = now->next;
        free(now);
    }

    free(list);
}
