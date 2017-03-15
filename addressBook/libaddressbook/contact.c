//
// Created by luknw on 3/11/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "contact.h"


static void *safeMalloc(size_t size) {
    void *allocated = malloc(size);
    if (allocated == NULL) {
        perror("Cannot allocate memory: ");
        exit(1);
    }
    return allocated;
}


Contact *Contact_new(size_t keyLen) {
    Contact *newContact = safeMalloc(sizeof(Contact));

    newContact->name = safeMalloc(keyLen);
    newContact->surname = safeMalloc(keyLen);
    newContact->birthDate = safeMalloc(keyLen);
    newContact->email = safeMalloc(keyLen);
    newContact->phone = safeMalloc(keyLen);
    newContact->address = safeMalloc(keyLen);

    return newContact;
}

void Contact_delete(Contact *contact) {
    free(contact->name);
    free(contact->surname);
    free(contact->birthDate);
    free(contact->email);
    free(contact->phone);
    free(contact->address);

    free(contact);
}

char *Contact_getValue(Contact *contact, ContactKey key) {
    assert(contact != NULL);

    switch (key) {
        case SURNAME:
            return contact->surname;
        case BIRTHDATE:
            return contact->birthDate;
        case EMAIL:
            return contact->email;
        case PHONE:
            return contact->phone;
    }
    return NULL;
}

int Contact_compare(Contact *a, Contact *b, ContactKey key) {
    assert(a != NULL);
    assert(b != NULL);

    return strcmp(Contact_getValue(a, key), Contact_getValue(b, key));
}
