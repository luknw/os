#ifdef DYNAMIC_LOADING

#include <dlfcn.h>

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "libaddressbook/addressBook.h"
#include "timer.h"


static const int CONTACT_COUNT = 1000;
static const int BUFFER_SIZE = 128;
static const char *DELIMITERS = ",\n";


static Contact *(*pContact_new)(size_t);

static void (*pContact_delete)(Contact *);

static char *(*pContact_getValue)(Contact *, ContactKey);


static ListAddressBook *(*pListAddressBook_new)(void);

static void (*pListAddressBook_addContact)(ListAddressBook *, Contact *);

static Contact *(*pListAddressBook_findContact)(ListAddressBook *, ContactKey, char *);

static void (*pListAddressBook_rearrangeByKey)(ListAddressBook **, ContactKey);

static void (*pListAddressBook_removeContact)(ListAddressBook *, Contact *);

static void (*pListAddressBook_delete)(ListAddressBook *);


static TreeAddressBook *(*pTreeAddressBook_new)(void);

static void (*pTreeAddressBook_addContact)(TreeAddressBook *, Contact *);

static Contact *(*pTreeAddressBook_findContact)(TreeAddressBook *, ContactKey, char *);

static void (*pTreeAddressBook_rearrangeByKey)(TreeAddressBook **, ContactKey);

static void (*pTreeAddressBook_removeContact)(TreeAddressBook *, Contact *);

static void (*pTreeAddressBook_delete)(TreeAddressBook *);


static void *loadSymbols() {
#ifdef DYNAMIC_LOADING
    void *symbols = dlopen("libaddressbook/libaddressbook.so", RTLD_LAZY);

    pContact_new = dlsym(symbols, "Contact_new");
    pContact_delete = dlsym(symbols, "Contact_delete");
    pContact_getValue = dlsym(symbols, "Contact_getValue");

    pListAddressBook_new = dlsym(symbols, "ListAddressBook_new");
    pListAddressBook_addContact = dlsym(symbols, "ListAddressBook_addContact");
    pListAddressBook_findContact = dlsym(symbols, "ListAddressBook_findContact");
    pListAddressBook_rearrangeByKey = dlsym(symbols, "ListAddressBook_rearrangeByKey");
    pListAddressBook_removeContact = dlsym(symbols, "ListAddressBook_removeContact");
    pListAddressBook_delete = dlsym(symbols, "ListAddressBook_delete");

    pTreeAddressBook_new = dlsym(symbols, "TreeAddressBook_new");
    pTreeAddressBook_addContact = dlsym(symbols, "TreeAddressBook_addContact");
    pTreeAddressBook_findContact = dlsym(symbols, "TreeAddressBook_findContact");
    pTreeAddressBook_rearrangeByKey = dlsym(symbols, "TreeAddressBook_rearrangeByKey");
    pTreeAddressBook_removeContact = dlsym(symbols, "TreeAddressBook_removeContact");
    pTreeAddressBook_delete = dlsym(symbols, "TreeAddressBook_delete");

    return symbols;
#else
    pContact_new = Contact_new;
    pContact_delete = Contact_delete;
    pContact_getValue = Contact_getValue;

    pListAddressBook_new = ListAddressBook_new;
    pListAddressBook_addContact = ListAddressBook_addContact;
    pListAddressBook_findContact = ListAddressBook_findContact;
    pListAddressBook_rearrangeByKey = ListAddressBook_rearrangeByKey;
    pListAddressBook_removeContact = ListAddressBook_removeContact;
    pListAddressBook_delete = ListAddressBook_delete;

    pTreeAddressBook_new = TreeAddressBook_new;
    pTreeAddressBook_addContact = TreeAddressBook_addContact;
    pTreeAddressBook_findContact = TreeAddressBook_findContact;
    pTreeAddressBook_rearrangeByKey = TreeAddressBook_rearrangeByKey;
    pTreeAddressBook_removeContact = TreeAddressBook_removeContact;
    pTreeAddressBook_delete = TreeAddressBook_delete;

    return NULL;
#endif
}


static void parseContact(char *raw, Contact *parsed) {
    strcpy(parsed->name, strtok(raw, DELIMITERS));
    strcpy(parsed->surname, strtok(NULL, DELIMITERS));
    strcpy(parsed->birthDate, strtok(NULL, DELIMITERS));
    strcpy(parsed->email, strtok(NULL, DELIMITERS));
    strcpy(parsed->phone, strtok(NULL, DELIMITERS));
    strcpy(parsed->address, strtok(NULL, DELIMITERS));
}

static void loadContacts(Contact **contacts) {
    char *buffer = malloc(BUFFER_SIZE);
    FILE *data = fopen("data.csv", "r");

    for (int i = 0; i < CONTACT_COUNT; ++i) {
        fgets(buffer, BUFFER_SIZE, data);
        contacts[i] = (*pContact_new)(DEFAULT_KEY_LEN);
        parseContact(buffer, contacts[i]);
    }

    fclose(data);
    free(buffer);
}

static void cleanup(Contact **contacts) {
    for (int i = 0; i < CONTACT_COUNT; ++i) {
        (*pContact_delete)(contacts[i]);
    }
    free(contacts);
}


static void benchmarkListAddressBook(ListAddressBook **book, Contact **contacts) {
    INIT_MEASURE_TIME()

    MEASURE_TIME("ListAddressBook - creating", *book = (*pListAddressBook_new)();)

    TimingInfo avgAddingTime = TimingInfo_new();

    MEASURE_TIME("ListAddressBook - adding first contact", (*pListAddressBook_addContact)(*book, contacts[0]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("ListAddressBook - adding almost all contacts",
                 for (int i = 1; i < CONTACT_COUNT - 1; ++i) {
                     (*pListAddressBook_addContact)(*book, contacts[i]);

                 })
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("ListAddressBook - adding last contact",
                 (*pListAddressBook_addContact)(*book, contacts[CONTACT_COUNT - 1]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    avgAddingTime = TimingInfo_divLong(avgAddingTime, CONTACT_COUNT);
    TimingInfo_print("ListAddressBook - adding average", avgAddingTime);

    MEASURE_TIME("ListAddressBook - finding optimistic",
                 (*pListAddressBook_findContact)(*book, (*book)->key,
                                                 (*pContact_getValue)((*book)->contacts->next->contact, (*book)->key));)

    MEASURE_TIME("ListAddressBook - finding pessimistic",
                 (*pListAddressBook_findContact)(*book, (*book)->key,
                                                 (*pContact_getValue)((*book)->contacts->prev->contact, (*book)->key));)

    MEASURE_TIME("ListAddressBook - rearranging", (*pListAddressBook_rearrangeByKey)(book, PHONE);)

    MEASURE_TIME("ListAddressBook - deleting pessimistic",
                 (*pListAddressBook_removeContact)(*book, (*book)->contacts->prev->contact);)

    MEASURE_TIME("ListAddressBook - deleting optimistic",
                 (*pListAddressBook_removeContact)(*book, (*book)->contacts->next->contact);)

    printf("\n");
}

static void benchmarkTreeAddressBook(TreeAddressBook **book, Contact **contacts) {
    INIT_MEASURE_TIME()

    MEASURE_TIME("TreeAddressBook - creating", *book = (*pTreeAddressBook_new)();)

    TimingInfo avgAddingTime = TimingInfo_new();

    MEASURE_TIME("TreeAddressBook - adding first contact", (*pTreeAddressBook_addContact)(*book, contacts[0]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("TreeAddressBook - adding almost all contacts",
                 for (int i = 1; i < CONTACT_COUNT - 1; ++i) {
                     (*pTreeAddressBook_addContact)(*book, contacts[i]);
                 })
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("TreeAddressBook - adding last contact",
                 (*pTreeAddressBook_addContact)(*book, contacts[CONTACT_COUNT - 1]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    avgAddingTime = TimingInfo_divLong(avgAddingTime, CONTACT_COUNT);
    TimingInfo_print("TreeAddressBook - adding average", avgAddingTime);

    MEASURE_TIME("TreeAddressBook - finding optimistic",
                 (*pTreeAddressBook_findContact)(*book, (*book)->key,
                                                 (*pContact_getValue)((*book)->contacts->contact, (*book)->key));)

    MEASURE_TIME("TreeAddressBook - deleting optimistic",
                 (*pTreeAddressBook_removeContact)(*book, (*book)->contacts->contact);)

    MEASURE_TIME("TreeAddressBook - rearranging", (*pTreeAddressBook_rearrangeByKey)(book, PHONE);)

    MEASURE_TIME("TreeAddressBook - deallocating", (*pTreeAddressBook_delete)(*book);)

    *book = (*pTreeAddressBook_new)();
    MEASURE_TIME("TreeAddressBook - adding pessimistic data",
                 for (int i = 0; i < CONTACT_COUNT - 1; ++i) {
                     (*pTreeAddressBook_addContact)(*book, contacts[0]);
                 }
                         (*pTreeAddressBook_addContact)(*book, contacts[2]);)

    MEASURE_TIME("TreeAddressBook - finding pessimistic",
                 (*pTreeAddressBook_findContact)(*book, (*book)->key,
                                                 (*pContact_getValue)(contacts[2], (*book)->key));)

    MEASURE_TIME("TreeAddressBook - deleting pessimistic",
                 (*pTreeAddressBook_removeContact)(*book, contacts[2]);)

    printf("\n");
}


int main(void) {
#ifdef DYNAMIC_LOADING
    void *symbols = loadSymbols();
#else
    loadSymbols();
#endif

    Contact **contacts = calloc(CONTACT_COUNT, sizeof(Contact *));
    loadContacts(contacts);

    ListAddressBook *listBook = NULL;
    benchmarkListAddressBook(&listBook, contacts);

    TreeAddressBook *treeBook = NULL;
    benchmarkTreeAddressBook(&treeBook, contacts);

    (*pListAddressBook_delete)(listBook);
    (*pTreeAddressBook_delete)(treeBook);

    cleanup(contacts);

#ifdef DYNAMIC_LOADING
    dlclose(symbols);
#endif

    return 0;
}
