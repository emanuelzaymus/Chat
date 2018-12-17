#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory>
#include <pthread.h>
#include <iostream>

class Client {
public:
    Client(const char* hostName, int port);
    virtual ~Client();

private:
    static int sockfd;
    static int n;                   //todo delete n
    static char buffer[256];

    static bool running;

    struct sockaddr_in serv_addr;
    struct hostent* server;

    static pthread_t reading;
    static pthread_t writing;

    static pthread_mutex_t mutex;   //todo not used
    static pthread_cond_t cond;     //todo not used

    static void* readMsg(void* ptr);    //output
    static void* writeMsg(void* ptr);   //input

    static void disconnect();
};

#endif /* CLIENT_H */

