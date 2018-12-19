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

using namespace std;

class Client {
public:
    Client(const char* hostName, int port);
    virtual ~Client();
    void run();

private:
    static int sockfd;
    static char buffer[256];
    static bool running;
    static pthread_t reading;
    static pthread_t writing;

    struct sockaddr_in serv_addr;
    struct hostent* server;

    static void* readMsgs(void* ptr);
    static void* writeMsgs(void* ptr);
    static void disconnect();
    static string readln();
    static void writeToServer();
    static void writeToServer(string str);
    static void readFromServer();

    void getFriendsList();
    void makeChice();
    void login();
};

#endif /* CLIENT_H */

