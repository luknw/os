//
// Created by luknw on 3/11/17.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "listAddressBook.h"


static ListContactNode *ListAddressBook_findLastLower(ListAddressBook *addressBook, Contact *contact) {
    ListContactNode *prev = addressBook->contacts;
    while (prev->next != addressBook->contacts
           && Contact_compare(contact, prev->next->contact, addressBook->key) > 0) {
        prev = prev->next;
    }
    return prev;
}

static ListContactNode *ListAddressBook_findNodeByValue(ListAddressBook *addressBook, ContactKey key, char *value) {
    ListContactNode *node = addressBook->contacts->next;

    if (addressBook->key == key) {
        while (node != addressBook->contacts && strcmp(value, Contact_getValue(node->contact, key)) > 0) {
            node = node->next;
        }
    } else {
        while (node != addressBook->contacts && strcmp(value, Contact_getValue(node->contact, key)) != 0) {
            node = node->next;
        }
    }

    return (node != addressBook->contacts && strcmp(value, Contact_getValue(node->contact, key)) == 0)
           ? node
           : NULL;
}

ListAddressBook *ListAddressBook_new(void) {
    ListAddressBook *addressBook = malloc(sizeof(ListAddressBook));
    if (addressBook == NULL) {
        perror("Cannot allocate address book: ");
        exit(1);
    }

    addressBook->key = DEFAULT_CONTACT_KEY;
    addressBook->size = 0;

    ListContactNode *guard = malloc(sizeof(ListContactNode));
    if (guard == NULL) {
        perror("Cannot allocate contact: ");
        exit(1);
    }
    guard->contact = NULL;
    guard->prev = guard;
    guard->next = guard;

    addressBook->contacts = guard;

    return addressBook;
}

void ListAddressBook_delete(ListAddressBook *addressBook) {
    assert(addressBook != NULL);

    while (addressBook->size > 0) {
        ListAddressBook_removeContact(addressBook, addressBook->contacts->next->contact);
    }
    free(addressBook->contacts);

    free(addressBook);
}

void ListAddressBook_addContact(ListAddressBook *addressBook, Contact *added) {
    assert(addressBook != NULL);
    assert(added != NULL);

    addressBook->size += 1;

    ListContactNode *addedNode = malloc(sizeof(ListContactNode));
    if (addedNode == NULL) {
        perror("Cannot allocate contact: ");
        exit(1);
    }
    addedNode->contact = added;

    ListContactNode *beforeAdded = ListAddressBook_findLastLower(addressBook, added);

    addedNode->prev = beforeAdded;
    addedNode->next = beforeAdded->next;

    beforeAdded->next->prev = addedNode;
    beforeAdded->next = addedNode;
}

void ListAddressBook_removeContact(ListAddressBook *addressBook, Contact *removed) {
    assert(addressBook != NULL);
    assert(removed != NULL);

    ListContactNode *removedNode =
            ListAddressBook_findLastLower(addressBook, removed)->next;
    while (removedNode != addressBook->contacts
           && removedNode->contact != removed
           && Contact_compare(removed, removedNode->contact, addressBook->key) == 0) {
        removedNode = removedNode->next;
    }
    if (removedNode->contact != removed) return;

    addressBook->size -= 1;

    removedNode->next->prev = removedNode->prev;
    removedNode->prev->next = removedNode->next;

    free(removedNode);
}

Contact *ListAddressBook_findContact(ListAddressBook *addressBook, ContactKey key, char *value) {
    assert(addressBook != NULL);

    ListContactNode *node = ListAddressBook_findNodeByValue(addressBook, key, value);

    return (node != NULL)
           ? node->contact
           : NULL;
}

void ListAddressBook_rearrangeByKey(ListAddressBook **pAddressBook, ContactKey key) {
    assert(pAddressBook != NULL);
    assert(*pAddressBook != NULL);

    ListAddressBook *oldCopy = *pAddressBook;
    ListAddressBook *rearranged = ListAddressBook_new();
    rearranged->key = key;

    while (oldCopy->size > 0) {
        ListAddressBook_addContact(rearranged, oldCopy->contacts->next->contact);
        ListAddressBook_removeContact(oldCopy, oldCopy->contacts->next->contact);
    }

    ListAddressBook_delete(oldCopy);

    *pAddressBook = rearranged;
}
