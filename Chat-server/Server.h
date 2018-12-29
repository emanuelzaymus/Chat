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

class Server {
public:
    Server(int port);
    virtual ~Server();
    void run();

    static bool signIn(string nick, string password);
    static bool logIn(string nick, string password);
    static string getContacts(string nick);
    static bool addContact(string choseNick, string nick);
    static bool eraseContact(string choseNick, string nick);
    static bool isRunning();

private:
    static const string refisteredFile;

    static int port;
    static int sockfd;
    static socklen_t cli_len;
    static struct sockaddr_in cli_addr;
    static vector<Client*> clients;
    static bool running;

    struct sockaddr_in serv_addr;
    pthread_t consoleReader;
    pthread_t connecting;

    static void* connectClient(void* ptr);
    static void* readConsole(void* ptr);
    static void stopServer();
    static void connectLastClient();
    static void registerClient(string nick, string password);
    static bool isRegistered(string nick);
    static string passwordOf(string nick);
    static Client* findClientByNick(string nick);
    static bool hasContact(string chosenNick, string nick);
    static void writeContact(string choseNick, string nick);;
    static bool removeContact(string choseNick, string nick);;

    void stopAllClients();
};

#endif /* SERVER_H */

