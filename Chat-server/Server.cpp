#include <stddef.h>

#include "Server.h"

int Server::maxId = 0;

vector<struct client> Server::clients;
int Server::sockfd = 0;
socklen_t Server::cli_len = 0;
struct sockaddr_in Server::cli_addr = {0};

bool Server::running = true;

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
    cout << "in server destructor - start" << endl;


    close(sockfd);

    cout << "in server destructor - end" << endl;
}

void Server::run()
{
    cout << "start running server" << endl;

    pthread_create(&consoleReader, NULL, &readConsole, NULL);
    pthread_create(&connecting, NULL, &connectClient, NULL);

    //    pthread_join(connecting, NULL);   // TODO: never joined !!!
    pthread_join(consoleReader, NULL);

    stopAllClients();

    struct client cl;
    while (clients.size() != 0)
    {
        cl = clients.back();
        pthread_join(cl.running, NULL);
        close(cl.newsockfd);
        clients.pop_back();
    }

    cout << "end running server" << endl;
}

void* Server::connectClient(void* ptr)
{
    int newSockfd;
    while (running)
    {
        newSockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);
        if (newSockfd < 0)
        {
            perror("ERROR on accept");
        }
        struct client c;
        c.newsockfd = newSockfd;
        c.id = maxId++;
        clients.push_back(c);

        pthread_create(&(clients.back().running), NULL, &runClient, &clients.back());
        cout << "connectClient - created client " << c.id << endl;
    }
    return nullptr;
}

void Server::stopClient(struct client* cl)
{
    if (cl->runningThreads)
    {
        bzero(cl->buffer, 256);
        cl->buffer[0] = 'S';
        cl->buffer[1] = 'T';
        cl->buffer[2] = 'O';
        cl->buffer[3] = 'P';
        cl->buffer[4] = '\n';

        write(cl->newsockfd, cl->buffer, strlen(cl->buffer) + 1);
        cl->runningThreads = false;

        cout << "client " << cl->id << " - stopped" << endl;
    }
}

void* Server::runClient(void* ptr)
{
    struct client cl = *((struct client*) ptr);
    int id = cl.id;

    cout << "start running client " << cl.id << endl;

    pthread_create(&cl.reading, NULL, &readMsg, &cl);

    pthread_join(cl.reading, NULL);

    close(cl.newsockfd);

    cout << "end running client " << id << endl;

    return nullptr;
}

void* Server::readMsg(void* ptr)
{
    struct client cl = *((struct client*) ptr);

    int n;
    while (running && cl.runningThreads)
    {
        bzero(cl.buffer, 256);
        n = read(cl.newsockfd, cl.buffer, 255);

        if (n < 0)
        {
            perror("Error reading from socket");
        }
        cout << "Client " << cl.id << ": " << cl.buffer;

        if (strcmp(cl.buffer, "end\n") == 0)
        {
            stopClient(&cl);
        }
    }
    cout << "readMsg - ended" << endl;
    return nullptr;
}

// todo writing only to 0. client

//void* Server::writeMsg(void* ptr)
//{
//    struct client cl = *((struct client*) ptr);
//
//    int n;
//    while (running && cl.runningThreads)
//    {
//        bzero(cl.buffer, 256);
//        fgets(cl.buffer, 255, stdin);
//        if (strcmp(cl.buffer, "stop\n") == 0)
//        {
//            stopServer();
//        }
//
//        n = write(cl.newsockfd, &cl.buffer, strlen(cl.buffer) + 1);
//
//        if (n < 0)
//        {
//            perror("Error writing to socket");
//        }
//    }
//    cout << "writeMsg - ended" << endl;
//    return nullptr;
//}

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
    cout << "server stopped !!!" << endl;
}

void Server::stopAllClients()
{
    for (auto cl : clients)
    {
        stopClient(&cl);
    }
}