//
// Created by luknw on 2017-06-07
//

#ifndef COPYRIGHT_CLIENT_H
#define COPYRIGHT_CLIENT_H

#include <string>
#include "ServerAddress.h"

using namespace std;


class Client {
public:
    Client(string rootDirectory, ServerAddress serverAddress, timespec syncInterval);

    void run();

    void sync();

protected:
    void openCache();

    void updateCache();

    void pullChanges();

    void pushChanges();

    pthread_t runDaemon();


    string rootDirectory;
    ServerAddress serverAddress;
    timespec syncInterval;
    pthread_t daemon;

private:
    static void *repeatSync(void *);
};


#endif //COPYRIGHT_CLIENT_H
