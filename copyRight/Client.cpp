//
// Created by luknw on 2017-06-07
//

#include "Client.h"

using namespace std;


Client::Client(string rootDirectory, ServerAddress serverAddress, timespec syncInterval)
        : rootDirectory(rootDirectory), serverAddress(serverAddress), syncInterval(syncInterval) {
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

}

void Client::updateCache() {

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
        nanosleep(&client->syncInterval, nullptr);
    }

    return nullptr;
}



