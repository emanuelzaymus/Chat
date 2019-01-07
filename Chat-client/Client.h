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
    static char bufferIn[2048];
    static char bufferOut[2048];
    static bool running;
    static bool runningWriting;
    static pthread_t reading;
    static pthread_t writing;

    static pthread_mutex_t mutexReading;
    static pthread_mutex_t mutexWriting;
    
    static bool repeatedLogging;
    static string choseNick;

    struct sockaddr_in serv_addr;
    struct hostent* server;

    static void runWritting();
    static void stopWritting();
    static void* readMsgs(void* ptr);
    static void* writeMsgs(void* ptr);
    static void disconnect();
    static string readln();
    static void writeToServer();
    static void writeToServer(string str);
    static void readFromServerWithCheck();
    static string readFromServer();

    static void startMenu();
    static void loggedInMenu();
    static void deleteAccount();
    static string getContacts();
    static void contactsMenu(bool wasMistake);
    static void addContact();
    static void tryAddContact();
    static void eraseContact();
    static void tryEraseContact();
    static void connectInChat();
    static void tryConnectInChat();
    static void startChat();
    static string getConversation();
    static void tryDisconnectInChat();
    static void logIn();
    static void tryLogIn();
    static void signIn();
    static void trySignIn();
    static void sendNickAndPassword(string nick, string password);

};

#endif /* CLIENT_H */

