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

using namespace std;

struct client {
    int newsockfd;
    int id;
    char buffer[256] = "";
    bool runningThreads = true;
    pthread_t reading;
    pthread_t running;
};

class Server {
public:
    Server(int port);
    virtual ~Server();
    void run();

private:
    static int maxId;
    
    static vector<struct client> clients;
    static int sockfd;
    static socklen_t cli_len;
    static struct sockaddr_in cli_addr;

    static bool running;

    struct sockaddr_in serv_addr;

    pthread_t consoleReader;
    pthread_t connecting;

    static void* connectClient(void* ptr);
    static void stopClient(struct client* cl);
    static void* runClient(void* ptr);
    static void* readMsg(void* ptr);    // todo rename to service
    static void* readConsole(void* ptr);
    
    static void stopServer();
    
    void stopAllClients();
};

#endif /* SERVER_H */

