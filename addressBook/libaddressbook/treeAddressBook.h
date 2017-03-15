//
// Created by luknw on 3/11/17.
//

#ifndef ADDRESSBOOK_TREE_ADDRESS_BOOK_H
#define ADDRESSBOOK_TREE_ADDRESS_BOOK_H

#include "contact.h"


typedef struct TreeContactNode TreeContactNode;
typedef struct TreeAddressBook TreeAddressBook;

struct TreeContactNode {
    Contact *contact;
    TreeContactNode *left;
    TreeContactNode *right;
    TreeContactNode *parent;
};

struct TreeAddressBook {
    TreeContactNode *contacts;
    ContactKey key;
    int size;
};

TreeAddressBook *TreeAddressBook_new(void);

void TreeAddressBook_delete(TreeAddressBook *addressBook);

void TreeAddressBook_addContact(TreeAddressBook *addressBook, Contact *added);

void TreeAddressBook_removeContact(TreeAddressBook *addressBook, Contact *contact);

Contact *TreeAddressBook_findContact(TreeAddressBook *addressBook, ContactKey key, char *value);

void TreeAddressBook_rearrangeByKey(TreeAddressBook **addressBook, ContactKey key);

#endif //ADDRESSBOOK_TREE_ADDRESS_BOOK_H
