//
// Created by luknw on 3/11/17.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "treeAddressBook.h"


static void TreeAddressBook_deleteSubTree(TreeContactNode *root) {
    if (root == NULL) return;

    TreeAddressBook_deleteSubTree(root->left);
    TreeAddressBook_deleteSubTree(root->right);

    free(root);
}

TreeAddressBook *TreeAddressBook_new(void) {
    TreeAddressBook *addressBook = malloc(sizeof(TreeAddressBook));
    if (addressBook == NULL) {
        perror("Cannot allocate address book: ");
        exit(1);
    }

    addressBook->contacts = NULL;
    addressBook->key = DEFAULT_CONTACT_KEY;
    addressBook->size = 0;

    return addressBook;
}

void TreeAddressBook_delete(TreeAddressBook *addressBook) {
    assert(addressBook != NULL);

    TreeAddressBook_deleteSubTree(addressBook->contacts);

    free(addressBook);
}

static void TreeAddressBook_insertNode(TreeAddressBook *addressBook, TreeContactNode *inserted, ContactKey key) {
    TreeContactNode *parent = NULL;
    TreeContactNode *child = addressBook->contacts;

    while (child != NULL) {
        parent = child;
        if (Contact_compare(inserted->contact, child->contact, key) >= 0) {
            child = child->right;
        } else {
            child = child->left;
        }
    }

    inserted->parent = parent;

    if (parent == NULL) {
        addressBook->contacts = inserted;
    } else if (Contact_compare(inserted->contact, parent->contact, key) >= 0) {
        parent->right = inserted;
    } else {
        parent->left = inserted;
    }
}

void TreeAddressBook_addContact(TreeAddressBook *addressBook, Contact *added) {
    assert(addressBook != NULL);
    assert(added != NULL);

    addressBook->size += 1;

    TreeContactNode *addedNode = malloc(sizeof(TreeContactNode));
    if (addedNode == NULL) {
        perror("Cannot allocate contact: ");
        exit(1);
    }
    addedNode->contact = added;
    addedNode->left = NULL;
    addedNode->right = NULL;

    TreeAddressBook_insertNode(addressBook, addedNode, addressBook->key);
}

static TreeContactNode *
TreeAddressBook_findContactNode(TreeContactNode *candidateRoot, ContactKey key, Contact *contact) {
    while (candidateRoot != NULL && candidateRoot->contact != contact) {
        candidateRoot = (Contact_compare(contact, candidateRoot->contact, key) >= 0)
                        ? candidateRoot->right
                        : candidateRoot->left;
    }
    return candidateRoot;
}

static TreeContactNode *TreeAddressBook_findMin(TreeContactNode *candidateRoot, ContactKey key) {
    if (candidateRoot == NULL) return NULL;

    while (candidateRoot->left != NULL) {
        candidateRoot = candidateRoot->left;
    }

    return candidateRoot;
}

void TreeAddressBook_removeContact(TreeAddressBook *addressBook, Contact *removed) {
    assert(addressBook != NULL);
    assert(removed != NULL);

    TreeContactNode *effectivelyRemoved =
            TreeAddressBook_findContactNode(addressBook->contacts, addressBook->key, removed);

    if (effectivelyRemoved == NULL) return;

    TreeContactNode *removedNode = (effectivelyRemoved->left == NULL || effectivelyRemoved->right == NULL)
                                   ? effectivelyRemoved
                                   : TreeAddressBook_findMin(effectivelyRemoved->right, addressBook->key);

    TreeContactNode *replacement = (removedNode->left != NULL)
                                   ? removedNode->left
                                   : removedNode->right;

    if (replacement != NULL) {
        replacement->parent = removedNode->parent;
    }

    if (removedNode->parent == NULL) {
        addressBook->contacts = replacement;
    } else if (removedNode == removedNode->parent->left) {
        removedNode->parent->left = replacement;
    } else {
        removedNode->parent->right = replacement;
    }

    if (removedNode != effectivelyRemoved) {
        effectivelyRemoved->contact = removedNode->contact;
    }

    addressBook->size -= 1;
    free(removedNode);
}

static Contact *TreeAddressBook_findContactByValue(TreeContactNode *candidateRoot, ContactKey key, char *value) {
    int cmp;
    while (candidateRoot != NULL && (cmp = strcmp(value, Contact_getValue(candidateRoot->contact, key))) != 0) {
        candidateRoot = (cmp > 0) ? candidateRoot->right : candidateRoot->left;
    }
    return (candidateRoot != NULL) ? candidateRoot->contact : NULL;
}

Contact *TreeAddressBook_findContact(TreeAddressBook *addressBook, ContactKey key, char *value) {
    assert(addressBook != NULL);

    return TreeAddressBook_findContactByValue(addressBook->contacts, key, value);
}

void TreeAddressBook_rearrangeByKey(TreeAddressBook **pAddressBook, ContactKey key) {
    assert(pAddressBook != NULL);
    assert(*pAddressBook != NULL);

    TreeAddressBook *oldCopy = *pAddressBook;
    TreeAddressBook *rearranged = TreeAddressBook_new();
    rearranged->key = key;

    while (oldCopy->size > 0) {
        TreeAddressBook_addContact(rearranged, oldCopy->contacts->contact);
        TreeAddressBook_removeContact(oldCopy, oldCopy->contacts->contact);
    }

    TreeAddressBook_delete(oldCopy);

    *pAddressBook = rearranged;
}