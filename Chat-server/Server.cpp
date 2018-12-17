#include "Server.h"

int Server::newsockfd = 0;
int Server::n = 0;
char Server::buffer[256] = {0};

Server::Server(char *argv[])
{
    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

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

    newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);

    if (newsockfd < 0)
    {
        perror("ERROR on accept");
    }

    pthread_create(&reading, NULL, &readMsg, NULL);
    pthread_create(&writing, NULL, &writeMsg, NULL);
}

Server::Server(const Server& orig)
{
}

Server::~Server()
{
    pthread_join(reading, NULL);
    pthread_join(writing, NULL);

    close(newsockfd);
    close(sockfd);
}

void* Server::readMsg(void* ptr)
{
    while (true)
    {
        printf("1");
        bzero(Server::buffer, 256);
        printf("2");
        n = read(newsockfd, Server::buffer, 255);
        printf("3");

        if (Server::n < 0)
        {
            perror("Error reading from socket");
        }
        printf("4");
        printf("Here is the message: %s\n", Server::buffer);
        printf("5");
    }

    return nullptr;
}

void* Server::writeMsg(void* ptr)
{
    while (true)
    {
        printf("Please enter a message: ");
        printf("{2}");

        bzero(Server::buffer, 256);
        printf("{3}");

        fgets(Server::buffer, 255, stdin);
        printf("{4}");

        Server::n = write(Server::newsockfd, &Server::buffer, strlen(Server::buffer) + 1);

        if (Server::n < 0)
        {
            perror("Error writing to socket");
        }
    }

    return nullptr;
}