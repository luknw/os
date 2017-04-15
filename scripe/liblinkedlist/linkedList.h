//
// Created by luknw on 4/15/17.
//

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <stdbool.h>

#include "../libsafe/safe.h"


typedef struct LinkedListNode LinkedListNode;
typedef struct LinkedList LinkedList;

struct LinkedListNode {
    void *object;

    LinkedListNode *prev;
    LinkedListNode *next;
};

struct LinkedList {
    LinkedListNode *front;
    LinkedListNode *back;

    size_t len;
};


LinkedList *LinkedList_new();

void LinkedList_delete(LinkedList *q);

bool LinkedList_add(LinkedList *q, void *object);

void *LinkedList_remove(void *object);

bool LinkedList_isEmpty(LinkedList *q);


#endif //LINKEDLIST_H
