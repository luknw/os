#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <librsync.h>
#include "Client.h"

static const char *const OLD = "res/old.txt";
static const char *const OLD_SIG = "res/old.sig";
static const char *const NEW = "res/new.txt";
static const char *const DELTA = "res/delta.bin";
static const char *const UPDATED = "res/updated.txt";

using namespace std;


void checkRsyncStatus(const rs_result &status) {
    if (status == RS_DONE) {
        cout << "Done" << endl;
    } else {
        cerr << "Error" << endl;
        exit(EXIT_FAILURE);
    }
}


int rsyncTest() {
    cout << rs_librsync_version << endl;

    FILE *oldFile = fopen(OLD, "rb");
    FILE *oldSigFile = fopen(OLD_SIG, "wb");

    checkRsyncStatus(rs_sig_file(oldFile,
                                 oldSigFile,
                                 RS_DEFAULT_BLOCK_LEN,
                                 RS_MAX_STRONG_SUM_LENGTH,
                                 RS_BLAKE2_SIG_MAGIC,
                                 NULL));
    fclose(oldSigFile);


    oldSigFile = fopen(OLD_SIG, "rb");
    rs_signature_t *oldSig;

    checkRsyncStatus(rs_loadsig_file(oldSigFile, &oldSig, NULL));
    checkRsyncStatus(rs_build_hash_table(oldSig));

    fclose(oldSigFile);


    FILE *newFile = fopen(NEW, "rb");
    FILE *deltaFile = fopen(DELTA, "wb");

    checkRsyncStatus(rs_delta_file(oldSig, newFile, deltaFile, NULL));

    fclose(deltaFile);
    fclose(newFile);


    deltaFile = fopen(DELTA, "rb");
    FILE *updatedFile = fopen(UPDATED, "wb");

    checkRsyncStatus(rs_patch_file(oldFile, deltaFile, updatedFile, NULL));

    fclose(updatedFile);
    fclose(deltaFile);


    return 0;
}

int directoryCreationTest() {
    ServerAddress serverAddress;
    timespec syncInterval = {1, 0};
    Client *forest = new Client("./res", serverAddress, syncInterval);

    forest->run();

    return 0;
}

int main() {
    cout << 2 % 3 << " " << 3 % 2;
//    rsyncTest();
//    directoryCreationTest();
//
//    pthread_exit(NULL);
}
