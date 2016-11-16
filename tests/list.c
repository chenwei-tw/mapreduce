#include "list.h"
#include <stdio.h>
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

int llist_pop_front(llist_t *list)
{
    node_t *node;
    int data;

    if ((list == NULL) || (list->size == 0))
        return -1;

    node = list->head;
    data = node->data;

    list->head = list->head->next;
    list->size--;

    free(node);

    return data;
}

int llist_size(llist_t *list)
{
    if (list == NULL)
        return 0;

    return list->size;
}

void llist_print(llist_t *list)
{
    node_t *node;

    if (list == NULL)
        return;

    node = list->head;
    while (node) {
        printf("%d\n", node->data);
        node = node->next;
    }
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
