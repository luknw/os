//
// Created by luknw on 4/15/17.
//

#include "linkedList.h"


static LinkedListNode *LinkedListNode_new(void *object, LinkedListNode *prev, LinkedListNode *next) {
    LinkedListNode *node = safe_malloc(sizeof(LinkedListNode));

    node->object = object;
    node->prev = prev;
    node->next = next;

    return node;
}

static void LinkedListNode_delete(LinkedListNode *node) {
    safe_free(node);
}


static void LinkedList_insertBefore(LinkedList *list, void *object, LinkedListNode *insertedNext) {
    LinkedListNode *inserted = LinkedListNode_new(object, insertedNext->prev, insertedNext);

    insertedNext->prev->next = inserted;
    insertedNext->prev = inserted;

    (list->len)++;
}

void LinkedList_addFront(LinkedList *list, void *object) {
    LinkedList_insertBefore(list, object, list->front->next);
}

void LinkedList_addBack(LinkedList *list, void *object) {
    LinkedList_insertBefore(list, object, list->back);
}


static void *LinkedList_removeNode(LinkedList *list, LinkedListNode *removed) {
    if (removed->prev != NULL) removed->prev->next = removed->next;
    if (removed->next != NULL) removed->next->prev = removed->prev;

    (list->len)--;

    void *removedObject = removed->object;
    LinkedListNode_delete(removed);

    return removedObject;
}

void *LinkedList_removeFront(LinkedList *list) {
    if (list->len <= 0) return NULL;

    return LinkedList_removeNode(list, list->front->next);
}

void *LinkedList_removeBack(LinkedList *list) {
    if (list->len <= 0) return NULL;

    return LinkedList_removeNode(list, list->back->prev);
}


bool LinkedList_isEmpty(LinkedList *list) {
    return list->len == 0;
}


LinkedList *LinkedList_new() {
    LinkedList *list = safe_malloc(sizeof(LinkedList));

    list->front = LinkedListNode_new(NULL, NULL, NULL);
    list->back = LinkedListNode_new(NULL, list->front, NULL);
    list->front->next = list->back;

    list->len = 0;

    return list;
}

void LinkedList_delete(LinkedList *list) {
    while (list->len > 0) LinkedList_removeBack(list);

    safe_free(list->back);
    safe_free(list->front);

    safe_free(list);
}


#endif //LINKEDLIST_H
