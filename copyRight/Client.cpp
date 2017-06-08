//
// Created by luknw on 2017-06-07
//

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <iostream>

#include "Client.h"
#include <map>

using namespace std;


const string Client::CACHE_DIRECTORY = ".copyRightCache";
const string Client::CACHE_TIMESTAMP = "timestamp";


Client::Client(string rootDirectory, ServerAddress serverAddress, timespec syncInterval)
        : rootPath(rootDirectory), serverAddress(serverAddress), syncInterval(syncInterval) {
}

void Client::run() {
    openCache();
    runDaemon();
}

void Client::sync() {
    updateCache();
    pullChanges();
    pushChanges();
}

void Client::openCache() {
    cachePath = rootPath + "/" + CACHE_DIRECTORY;
    fdCache = createCacheIfDoesNotExist(cachePath);
}

int Client::createCacheIfDoesNotExist(string &cachePath) {
    int cache = open(cachePath.c_str(), O_RDONLY| O_DIRECTORY);
    if (cache == -1) {
        int err = errno;
        if (err == ENOTDIR) {
            cerr << "File named " << CACHE_DIRECTORY
                 << " conflicts with cache directory. Please rename it." << endl;
            exit(EXIT_FAILURE);
        }
        if (err != ENOENT) exit(EXIT_FAILURE);

        mkdir(cachePath.c_str(), 0755);
        cache = open(cachePath.c_str(), O_RDONLY);
    }
    return cache;
}

void Client::updateCache() {
    touchCacheTimestamp();

}

void Client::touchCacheTimestamp() {
    unlinkat(fdCache, CACHE_TIMESTAMP.c_str(), 0);
    openat(fdCache, CACHE_TIMESTAMP.c_str(), O_CREAT | O_EXCL, 0755);
}

void Client::pullChanges() {

}

void Client::pushChanges() {

}

pthread_t Client::runDaemon() {
    pthread_attr_t daemonAttr;
    pthread_attr_init(&daemonAttr);
    pthread_attr_setdetachstate(&daemonAttr, PTHREAD_CREATE_DETACHED);

    pthread_create(&daemon, nullptr, repeatSync, this);

    pthread_attr_destroy(&daemonAttr);

    return daemon;
}

void *Client::repeatSync(void *syncedClient) {
    Client *client = (Client *) syncedClient;

    bool ever = true;
    while (ever) {
        client->sync();
        cout << "." << flush;
        nanosleep(&client->syncInterval, nullptr);
    }

    return nullptr;
}
