#include <stddef.h>

#include "Server.h"

vector<struct client> Server::clients;
int Server::sockfd = 0;
socklen_t Server::cli_len = 0;
struct sockaddr_in Server::cli_addr = {0};

bool Server::running = true;

pthread_t Server::reading[256] = {0};
pthread_t Server::writing[256] = {0};

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

    pthread_create(&connecting, NULL, &connectClient, NULL);
}

Server::~Server()
{
    cout << "in server destructor - start" << endl;

    pthread_join(connecting, NULL);

    for (struct client cl : clients)
    {
        close(cl.newsockfd);
    }
    close(sockfd);

    cout << "in server destructor - end" << endl;
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
        c.index = clients.size();

        clients.push_back(c);
        pthread_create(&reading[c.index], NULL, &readMsg, &c);
        pthread_create(&writing[c.index], NULL, &writeMsg, &c);

        cout << "in connectClient - created (client " << c.index << ")" << endl;
    }
    return nullptr;
}

void Server::disconnectClient(struct client cl)
{
    cout << "in disconnectClient - start (client " << cl.index << ")" << endl;

    //todo bug here - stops here and waits
    pthread_join(reading[cl.index], NULL);
    pthread_join(writing[cl.index], NULL);

    close(cl.newsockfd);
    clients.erase(clients.begin() + cl.index);

    cout << "in disconnectClient - end (client " << cl.index << ")" << endl;
}

void* Server::readMsg(void* ptr)
{
    struct client cl = *((struct client*) ptr);

    int n;
    while (running && cl.runningThread)
    {
        while (clients.size() > 0)
        {
            bzero(cl.buffer, 256);
            n = read(cl.newsockfd, cl.buffer, 255);

            if (n < 0)
            {
                perror("Error reading from socket");
            }
            cout << "Client: " << cl.buffer;

            if (strcmp(cl.buffer, "end\n") == 0)
            {
                write(cl.newsockfd, &cl.buffer, strlen(cl.buffer) + 1);
                disconnectClient(cl);
            }
        }
    }
    return nullptr;
}

// todo writing only to 0. client
void* Server::writeMsg(void* ptr)
{
    struct client cl = *((struct client*) ptr);

    int n;
    while (running && cl.runningThread)
    {
        while (clients.size() > 0)
        {
//            cout << "You: ";

            bzero(cl.buffer, 256);
            fgets(cl.buffer, 255, stdin);
            n = write(cl.newsockfd, &cl.buffer, strlen(cl.buffer) + 1);

            if (n < 0)
            {
                perror("Error writing to socket");
            }
        }
    }
    return nullptr;
}

