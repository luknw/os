//
// Created by luknw on 3/11/17.
//

#ifndef ADDRESSBOOK_CONTACT_H
#define ADDRESSBOOK_CONTACT_H

#include <stdlib.h>

#define DEFAULT_CONTACT_KEY SURNAME
#define DEFAULT_KEY_LEN 128

typedef struct Contact Contact;

typedef enum ContactKey {
    SURNAME,
    BIRTHDATE,
    EMAIL,
    PHONE,
} ContactKey;

struct Contact {
    char *name;
    char *surname;
    char *birthDate;
    char *email;
    char *phone;
    char *address;
};

struct Contact *Contact_new(size_t keyLen);

void Contact_delete(Contact *contact);

char *Contact_getValue(Contact *contact, ContactKey key);

int Contact_compare(Contact *a, Contact *b, ContactKey key);

#endif //ADDRESSBOOK_CONTACT_H
