#ifndef SERVER_H
#define SERVER_H

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

#include "Client.h"

using namespace std;

struct client {
    int newsockfd;
    int id;
    char nick[256] = "";
    char buffer[256] = "";
    bool runningThreads = true;
    pthread_t reading;
    pthread_t running;
    struct client* chattingWith = nullptr;
};

class Server {
public:
    Server(int port);
    virtual ~Server();
    void run();

    static Client* findClientById(int id);

    static bool isRunning() {
        return running;
    }

private:
    static int sockfd;
    static socklen_t cli_len;
    static struct sockaddr_in cli_addr;
    static bool running;
    //    static vector<struct client*> clients;
    static vector<Client*> clients;
    static int maxId;

    struct sockaddr_in serv_addr;
    pthread_t consoleReader;
    pthread_t connecting;

    static void* connectClient(void* ptr);
    static void stopClient(struct client* cl);
    static void* runClient(void* ptr);
    static void* service(void* ptr);
    static void* readConsole(void* ptr);
    static void readFromClient(struct client* cl);
    static void readFromClientWithCheck(struct client* cl);
    static void stopServer();
    static void connectClients(struct client* cl);
    static void sendFriendList(struct client* cl);
    //    static struct client* findClientById(int id);
    static int readChoice(struct client* cl);
    static void sendToClient(char msg[256], char fromNick[256], struct client* toClient);
    static void sendToClient(string str, struct client* toClient);
    static void login(struct client* cl);

    static void connectLastClient();

    void stopAllClients();
};

#endif /* SERVER_H */

