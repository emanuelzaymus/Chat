#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <vector>

#include <netdb.h>
#include <memory>

//#include "Server.h" // may couse problems TODO !!!
//class Server;

using namespace std;

class Client {
public:
    Client(const int socket);
    virtual ~Client();

    struct clientData* getClientData() {
        return dat;
    }

    static void stop(struct clientData* data);
private:
    struct clientData* dat;
    static int maxId;
//    static Server* server;

    static void* run(void* ptr);
    static void* service(void* ptr);
    static void readFromClientWithCheck(struct clientData* data);
    static void readFromClient(struct clientData* data);
    static void connectClients(struct clientData* data);
    static void login(struct clientData* data);
    static void sendToClient(char msg[256], char fromNick[256], struct clientData* toClient);
    static void sendToClient(string str, struct clientData* toClient);
    static void sendFriendList(struct clientData* data);
    static int readChoice(struct clientData* data);

};

struct clientData {
    int socket;
    int id;
    char nick[256] = "";
    char buffer[256] = "";
    bool runningThreads = true;
    pthread_t reading;
    pthread_t running;
    Client* chattingWith = nullptr;
};

#endif /* CLIENT_H */

