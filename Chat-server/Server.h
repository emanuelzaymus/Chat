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
    int index;
    char buffer[256] = "";
    bool runningThread = true;
};

class Server {
public:
    Server(int port);
    virtual ~Server();

private:
    static vector<struct client> clients;
    static int sockfd;
    static socklen_t cli_len;
    static struct sockaddr_in cli_addr;

    static bool running;

    struct sockaddr_in serv_addr;

    pthread_t connecting;
    static pthread_t reading[256]; // todo give it to the struct (vector)
    static pthread_t writing[256]; // todo give it to the struct (vector)

    static void* connectClient(void* ptr);
    static void disconnectClient(struct client cl);
    static void* readMsg(void* ptr);
    static void* writeMsg(void* ptr);
};

#endif /* SERVER_H */

