//
// Created by luknw on 3/11/17.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "contact.h"


Contact *Contact_new(size_t keyLen) {
    Contact *newContact = malloc(sizeof(Contact));

    newContact->name = malloc(keyLen);
    newContact->surname = malloc(keyLen);
    newContact->birthDate = malloc(keyLen);
    newContact->email = malloc(keyLen);
    newContact->phone = malloc(keyLen);
    newContact->address = malloc(keyLen);

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
