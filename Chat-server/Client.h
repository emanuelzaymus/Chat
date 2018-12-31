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


using namespace std;

class Client {
public:
    Client(const int socket);
    virtual ~Client();

    struct clientData* getClientData();

    static void stop(struct clientData* data);

private:
    static int maxId;

    struct clientData* dat;

    static void* run(void* ptr);
    static void* service(void* ptr);
    static void readWithCheckFrom(struct clientData* data);
    static string readFrom(struct clientData* data);
    static void logIn(struct clientData* data);
    static void signIn(struct clientData* data);
    static void deleteAccount(struct clientData* data);
    static void getContacts(struct clientData* data);
    static void addContact(struct clientData* data);
    static void eraseContact(struct clientData* data);
    static void connectInChat(struct clientData* data);
    static void disconnectInChat(struct clientData* data);
    static void send(char msg[256], string fromNick, struct clientData* toClient);
    static void send(string msg, struct clientData* toClient);
    static void readNickAndPassword(string& nick, string& password, struct clientData* data);
    static void addNickToMsg(char msg[256], string fromNick);

};

struct clientData {
    int socket;
    int id;
    string nick = "";
    char buffer[256] = "";
    bool runningThreads = true;
    pthread_t reading;
    pthread_t running;
    Client* chattingWith;
};

#endif /* CLIENT_H */

