set(SOURCE_FILES addressBook.h
        contact.c contact.h
        listAddressBook.c listAddressBook.h
        treeAddressBook.c treeAddressBook.h)
add_library(addressBook ${SOURCE_FILES})
