#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "libAddressBook/addressBook.h"


static const int CONTACT_COUNT = 1000;
static const int BUFFER_SIZE = 128;
static const char *DELIMITERS = ",\n";
static const char *LOG_TIME_FORMAT = "%lf s\t\t%s\n";


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

double diffclock(clock_t end, clock_t start) {
    return (end - start) / (double) CLOCKS_PER_SEC;
}

void logTime(void (*action)(void), char *description) {
    clock_t start = clock();

    action();

    clock_t end = clock();

    printf(LOG_TIME_FORMAT, diffclock(end, start), description);
}

int main(void) {
    Contact **contacts = calloc(CONTACT_COUNT, sizeof(Contact *));
    loadContacts(contacts);

    clock_t start, end;

    ListAddressBook *listBook;

    void createListAddressBook(void) {
        listBook = ListAddressBook_new();
    }
    logTime(createListAddressBook, "ListAddressBook - creating");

    double time_interval;
    double avg_adding_time = 0;

    start = clock();
    ListAddressBook_addContact(listBook, contacts[0]);
    end = clock();
    time_interval = diffclock(end, start);
    logTime(time_interval, "ListAddressBook - adding first contact");
    avg_adding_time += time_interval;

    for (int i = 1; i < CONTACT_COUNT - 1; ++i) {
        start = clock();
        ListAddressBook_addContact(listBook, contacts[i]);
        end = clock();
        avg_adding_time += diffclock(end, start);
    }


    start = clock();
    ListAddressBook_addContact(listBook, contacts[0]);
    end = clock();
    time_interval = diffclock(end, start);
    logTime(time_interval, "ListAddressBook - adding last contact");
    avg_adding_time += time_interval;

    avg_adding_time /= CONTACT_COUNT;
    logTime(avg_adding_time, "ListAddressBook - adding average");

    start = clock();
    ListAddressBook_findContact(listBook, listBook->key,
                                Contact_getValue(listBook->contacts->next->contact, listBook->key));
    end = clock();
    logTime(diffclock(end, start), "ListAddressBook - finding optimistic");


    start = clock();
    ListAddressBook_findContact(listBook, listBook->key,
                                Contact_getValue(listBook->contacts->prev->contact, listBook->key));
    end = clock();
    logTime(diffclock(end, start), "ListAddressBook - finding pessimistic");

    ListAddressBook_delete(listBook);
    cleanup(contacts);
    return 0;
}
