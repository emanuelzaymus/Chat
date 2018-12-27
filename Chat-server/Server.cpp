#include <stddef.h>

#include "Server.h"


int Server::sockfd = 0;
socklen_t Server::cli_len = 0;
struct sockaddr_in Server::cli_addr = {0};
bool Server::running = true;
vector<struct client*> Server::clients;
int Server::maxId = 0;

Server::Server(int port)
{
    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
    }

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        perror("Error binding socket address");
    }

    listen(sockfd, 5);
    cli_len = sizeof (cli_addr);
}

Server::~Server()
{
    struct client* cl;
    while (clients.size() != 0)
    {
        cl = clients.back();
        pthread_join(cl->running, NULL);
        clients.pop_back();
        delete cl;
    }
    close(sockfd);
}

void Server::run()
{
    cout << " - - - SERVER STARTED - - - " << endl;

    pthread_create(&consoleReader, NULL, &readConsole, NULL);
    pthread_create(&connecting, NULL, &connectClient, NULL);

    pthread_join(connecting, NULL);
    pthread_join(consoleReader, NULL);

    stopAllClients();

    cout << " - - - SERVER STOPPED - - - " << endl;
}

void* Server::connectClient(void* ptr)
{
    int newSockfd;
    while (running)
    {
        newSockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);
        if (!running)
        {
            break;
        }
        if (newSockfd < 0)
        {
            perror("ERROR on accept");
        }
        struct client* c = new struct client();
        c->newsockfd = newSockfd;
        c->id = maxId++;
        clients.push_back(c);

        pthread_create(&c->running, NULL, &runClient, c);
        cout << "SERVER - Client " << c->id << " connected" << endl;
    }
    cout << "SERVER - end of connecting" << endl;
    return nullptr;
}

void Server::stopClient(struct client* cl)
{
    if (cl->runningThreads)
    {
        sendToClient(string("SERVER: STOP\n"), cl);
        bzero(cl->buffer, 256);
        cl->runningThreads = false;

        cout << "SERVER - Client " << cl->id << " stopped" << endl;
    }
}

void* Server::runClient(void* ptr)
{
    struct client* cl = (struct client*) ptr;
    int id = cl->id;

    pthread_create(&cl->reading, NULL, &service, cl);

    pthread_join(cl->reading, NULL);
    close(cl->newsockfd);

    cout << "SERVER - Client " << id << " disconnected" << endl;
    return nullptr;
}

void* Server::service(void* ptr)
{
    struct client* cl = (struct client*) ptr;

    while (running && cl->runningThreads)
    {
        readFromClientWithCheck(cl);
    }
    cout << "SERVER - Ended service for client " << cl->id << endl;
    return nullptr;
}

void Server::readFromClientWithCheck(struct client* cl)
{
    readFromClient(cl);

    if (strcmp(cl->buffer, "end\n") == 0)
    {
        stopClient(cl);
    }
    else if (strcmp(cl->buffer, "getFriendsList\n") == 0)
    {
        connectClients(cl);
    }
    else if (strcmp(cl->buffer, "login\n") == 0)
    {
        login(cl);
    }
    else
    {
        sendToClient(cl->buffer, cl->chattingWith);
    }
}

void Server::readFromClient(struct client* cl)
{
    bzero(cl->buffer, 256);
    int n = read(cl->newsockfd, cl->buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
    }
    cout << "Client " << cl->id << ": " << cl->buffer << flush;
}

void Server::sendToClient(char msg[256], struct client* toClient)
{
    int n = write(toClient->newsockfd, msg, strlen(msg) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
    cout << "SERVER - sent msg to client " << toClient->id << endl;
}

void Server::sendToClient(string str, struct client* toClient)
{
    bzero(toClient->buffer, 256);
    strcpy(toClient->buffer, str.c_str());
    sendToClient(toClient->buffer, toClient);
}

void* Server::readConsole(void* ptr)
{
    string command;
    while (running)
    {
        cin >> command;
        if (command == "stop")
        {
            stopServer();
        }
    }
    return nullptr;
}

void Server::stopServer()
{
    running = false;
    connectLastClient();
    cout << "SERVER is being stopped..." << endl;
}

void Server::stopAllClients()
{
    for (auto cl : clients)
    {
        stopClient(cl);
    }
}

void Server::connectClients(struct client* cl)
{
    do
    {
        sendFriendList(cl);
        cl->chattingWith = findClientById(readChoice(cl));
    }
    while (cl->chattingWith == nullptr);
}

void Server::sendFriendList(struct client* cl)
{
    bzero(cl->buffer, 256);
    strcpy(cl->buffer, "You do not have any contacts.\n");
    sendToClient(cl->buffer, cl);
}

int Server::readChoice(struct client* cl)
{
    readFromClient(cl);
    return atoi(cl->buffer);
}

struct client* Server::findClientById(int id)
{
    for (struct client* cl : clients)
    {
        if (cl->id == id)
        {
            return cl;
        }
    }
    return nullptr;
}

void Server::login(struct client* cl)
{
    readFromClient(cl);
    cout << endl;
    strcpy(cl->nick, cl->buffer);
}

void Server::connectLastClient()
{
    struct hostent* server = gethostbyname("frios2.fri.uniza.sk");
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
    }

    struct sockaddr_in serv_addr;

    bzero((char*) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
          (char*) server->h_addr,
          (char*) &serv_addr.sin_addr.s_addr,
          server->h_length
          );
    serv_addr.sin_port = htons(1046);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
    }

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        perror("Error connecting to socket");
    }
}