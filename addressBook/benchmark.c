#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libAddressBook/addressBook.h"
#include "timer.h"

static const int CONTACT_COUNT = 16000;
static const int BUFFER_SIZE = 128;
static const char *DELIMITERS = ",\n";


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
        contacts[i] = Contact_new(DEFAULT_KEY_LEN);
        parseContact(buffer, contacts[i]);
    }

    fclose(data);
    free(buffer);
}

static void cleanup(Contact **contacts) {
    for (int i = 0; i < CONTACT_COUNT; ++i) {
        Contact_delete(contacts[i]);
    }
    free(contacts);
}


static void benchmarkListAddressBook(ListAddressBook **book, Contact **contacts) {
    INIT_MEASURE_TIME()

    MEASURE_TIME("ListAddressBook - creating", *book = ListAddressBook_new();)

    TimingInfo avgAddingTime = TimingInfo_new();

    MEASURE_TIME("ListAddressBook - adding first contact", ListAddressBook_addContact(*book, contacts[0]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("ListAddressBook - adding almost all contacts",
                 for (int i = 1; i < CONTACT_COUNT - 1; ++i) {
                     ListAddressBook_addContact(*book, contacts[i]);

                 })
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("ListAddressBook - adding last contact",
                 ListAddressBook_addContact(*book, contacts[CONTACT_COUNT - 1]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    avgAddingTime = TimingInfo_divLong(avgAddingTime, CONTACT_COUNT);
    TimingInfo_print("ListAddressBook - adding average", avgAddingTime);

    MEASURE_TIME("ListAddressBook - finding optimistic",
                 ListAddressBook_findContact(*book, (*book)->key,
                                             Contact_getValue((*book)->contacts->next->contact, (*book)->key));)

    MEASURE_TIME("ListAddressBook - finding pessimistic",
                 ListAddressBook_findContact(*book, (*book)->key,
                                             Contact_getValue((*book)->contacts->prev->contact, (*book)->key));)

    MEASURE_TIME("ListAddressBook - rearranging", ListAddressBook_rearrangeByKey(book, PHONE);)

    MEASURE_TIME("ListAddressBook - deleting pessimistic",
                 ListAddressBook_removeContact(*book, (*book)->contacts->prev->contact);)

    MEASURE_TIME("ListAddressBook - deleting optimistic",
                 ListAddressBook_removeContact(*book, (*book)->contacts->next->contact);)

    printf("\n");
}

static void benchmarkTreeAddressBook(TreeAddressBook **book, Contact **contacts) {
    INIT_MEASURE_TIME()

    MEASURE_TIME("TreeAddressBook - creating", *book = TreeAddressBook_new();)

    TimingInfo avgAddingTime = TimingInfo_new();

    MEASURE_TIME("TreeAddressBook - adding first contact", TreeAddressBook_addContact(*book, contacts[0]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("TreeAddressBook - adding almost all contacts",
                 for (int i = 1; i < CONTACT_COUNT - 1; ++i) {
                     TreeAddressBook_addContact(*book, contacts[i]);
                 })
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    MEASURE_TIME("TreeAddressBook - adding last contact",
                 TreeAddressBook_addContact(*book, contacts[CONTACT_COUNT - 1]);)
    avgAddingTime = TimingInfo_add(avgAddingTime, _timingInfo);

    avgAddingTime = TimingInfo_divLong(avgAddingTime, CONTACT_COUNT);
    TimingInfo_print("TreeAddressBook - adding average", avgAddingTime);

    MEASURE_TIME("TreeAddressBook - finding optimistic",
                 TreeAddressBook_findContact(*book, (*book)->key,
                                             Contact_getValue((*book)->contacts->contact, (*book)->key));)

    MEASURE_TIME("TreeAddressBook - deleting optimistic",
                 TreeAddressBook_removeContact(*book, (*book)->contacts->contact);)

    MEASURE_TIME("TreeAddressBook - rearranging", TreeAddressBook_rearrangeByKey(book, PHONE);)

    MEASURE_TIME("TreeAddressBook - deallocating", TreeAddressBook_delete(*book);)

    *book = TreeAddressBook_new();
    MEASURE_TIME("TreeAddressBook - adding pessimistic data",
                 for (int i = 0; i < CONTACT_COUNT - 1; ++i) {
                     TreeAddressBook_addContact(*book, contacts[0]);
                 }
                         TreeAddressBook_addContact(*book, contacts[2]);)

    MEASURE_TIME("TreeAddressBook - finding pessimistic",
                 TreeAddressBook_findContact(*book, (*book)->key,
                                             Contact_getValue(contacts[2], (*book)->key));)

    MEASURE_TIME("TreeAddressBook - deleting pessimistic",
                 TreeAddressBook_removeContact(*book, contacts[2]);)

    printf("\n");
}


int main(void) {
    Contact **contacts = calloc(CONTACT_COUNT, sizeof(Contact *));
    loadContacts(contacts);

    ListAddressBook *listBook = NULL;
    benchmarkListAddressBook(&listBook, contacts);

    TreeAddressBook *treeBook = NULL;
    benchmarkTreeAddressBook(&treeBook, contacts);

    ListAddressBook_delete(listBook);
    TreeAddressBook_delete(treeBook);

    cleanup(contacts);

    return 0;
}
