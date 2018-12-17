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

class Client {
public:
    Client(char *argv[]);
    Client(const Client& orig);
    virtual ~Client();
private:
    static int sockfd;
    static int n;
    static char buffer[256];

    struct sockaddr_in serv_addr;
    struct hostent* server;

    pthread_t reading;
    pthread_t writing;

    static void* readMsg(void* ptr);
    static void* writeMsg(void* ptr);
};

#endif /* CLIENT_H */

