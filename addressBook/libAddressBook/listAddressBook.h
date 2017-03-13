//
// Created by luknw on 3/11/17.
//

#ifndef ADDRESSBOOK_LIST_ADDRESS_BOOK_H
#define ADDRESSBOOK_LIST_ADDRESS_BOOK_H

#include "contact.h"


typedef struct ListContactNode ListContactNode;
typedef struct ListAddressBook ListAddressBook;

struct ListContactNode {
    Contact *contact;
    ListContactNode *prev;
    ListContactNode *next;
};

struct ListAddressBook {
    ListContactNode *contacts;
    ContactKey key;
    int size;
};

ListAddressBook *ListAddressBook_new(void);

void ListAddressBook_delete(ListAddressBook *addressBook);

void ListAddressBook_addContact(ListAddressBook *addressBook, struct Contact *added);

void ListAddressBook_removeContact(ListAddressBook *addressBook, struct Contact *contact);

struct Contact *ListAddressBook_findContact(ListAddressBook *addressBook, enum ContactKey key, char *value);

void ListAddressBook_rearrangeByKey(ListAddressBook **addressBook, ContactKey key);

#endif //ADDRESSBOOK_LIST_ADDRESS_BOOK_H
