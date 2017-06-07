#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <librsync.h>

using namespace std;


void checkRsyncStatus(const rs_result &status) {
    if (status == RS_DONE) {
        cout << "Done" << endl;
    } else {
        cerr << "Error" << endl;
        exit(EXIT_FAILURE);
    }
}


int main() {
    cout << rs_librsync_version << endl;

    FILE *oldFile = fopen("res/old.txt", "rb");
    FILE *oldSigFile = fopen("res/old.sig", "wb");

    checkRsyncStatus(rs_sig_file(oldFile,
                                 oldSigFile,
                                 RS_DEFAULT_BLOCK_LEN,
                                 RS_MAX_STRONG_SUM_LENGTH,
                                 RS_BLAKE2_SIG_MAGIC,
                                 NULL));
    fclose(oldSigFile);


    oldSigFile = fopen("res/old.sig", "rb");
    rs_signature_t *oldSig;

    checkRsyncStatus(rs_loadsig_file(oldSigFile, &oldSig, NULL));
    checkRsyncStatus(rs_build_hash_table(oldSig));

    fclose(oldSigFile);


    FILE *newFile = fopen("res/new.txt", "rb");
    FILE *deltaFile = fopen("res/delta.bin", "wb");

    checkRsyncStatus(rs_delta_file(oldSig, newFile, deltaFile, NULL));

    fclose(deltaFile);
    fclose(newFile);


    deltaFile = fopen("res/delta.bin", "rb");
    FILE *updatedFile = fopen("res/updated.txt", "wb");

    checkRsyncStatus(rs_patch_file(oldFile, deltaFile, updatedFile, NULL));

    fclose(updatedFile);
    fclose(deltaFile);


    return 0;
}
