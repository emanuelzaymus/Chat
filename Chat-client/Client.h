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

#include "CLI.h"

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

    static pthread_mutex_t mutex;
    static pthread_cond_t cond;
    static bool isLocked;
    static bool repeatedLogging;

    struct sockaddr_in serv_addr;
    struct hostent* server;

    static void* readMsgs(void* ptr);
    static void* writeMsgs(void* ptr);
    static void disconnect();
    static string readln();
    static void writeToServer();
    static void writeToServer(string str);
    static void readFromServerWithCheck();
    static string readFromServer();

    static void getContacts();
    static void contactsMenu(string contacts);
    static void logIn();
    static void tryLogIn();
    static void signIn();
    static void trySignIn();
    static void sendNickAndPassword(string nick, string password);
    static void startMenu();
    static void loggedInMenu();
    void runWritting();

};

#endif /* CLIENT_H */

