#include <stddef.h>

#include "Server.h"


int Server::sockfd = 0;
socklen_t Server::cli_len = 0;
struct sockaddr_in Server::cli_addr = {0};
bool Server::running = true;
vector<Client*> Server::clients;

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
    Client* cl;
    while (clients.size() != 0)
    {
        cl = clients.back();
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
        Client* c = new Client(newSockfd);
        clients.push_back(c);
    }
    cout << "SERVER - end of connecting clients" << endl;
    return nullptr;
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
    for (Client* cl : clients)
    {
        cl->stop(cl->getClientData());
    }
}

Client* Server::findClientById(int id)
{
    for (Client* cl : clients)
    {
        if (cl->getClientData()->id == id)
        {
            return cl;
        }
    }
    return nullptr;
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
