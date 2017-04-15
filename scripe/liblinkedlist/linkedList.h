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

    size_t size;
};


void LinkedList_addFront(LinkedList *list, void *object);

void LinkedList_addBack(LinkedList *list, void *object);


void *LinkedList_removeFront(LinkedList *list);

void *LinkedList_removeBack(LinkedList *list);

void *LinkedList_remove(LinkedList *list, void *object);


bool LinkedList_isEmpty(LinkedList *list);


LinkedList *LinkedList_new();

void LinkedList_delete(LinkedList *list);


#endif //LINKEDLIST_H
