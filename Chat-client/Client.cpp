#include "Client.h"

int Client::sockfd = 0;
int Client::n = 0;
char Client::buffer[256] = {0};

Client::Client(char *argv[])
{
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
    }

    bzero((char*) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
          (char*) server->h_addr,
          (char*) &serv_addr.sin_addr.s_addr,
          server->h_length
          );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
    }

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        perror("Error connecting to socket");
    }

    pthread_create(&reading, NULL, &readMsg, NULL);
    pthread_create(&writing, NULL, &writeMsg, NULL);
}

Client::Client(const Client& orig)
{
}

Client::~Client()
{
    pthread_join(reading, NULL);
    pthread_join(writing, NULL);

    printf("%s\n", buffer);
    close(sockfd);
}

void* Client::readMsg(void* ptr)
{
    while (true)
    {
        bzero(Client::buffer, 256);
        printf("7");

        n = read(Client::sockfd, Client::buffer, 255);
        printf("8");
        printf("Here is the message: %s\n", Client::buffer);
        printf("9");

        if (n < 0)
        {
            perror("Error reading from socket");
        }
    }

    return nullptr;
}

void* Client::writeMsg(void* ptr)
{
    while (true)
    {
        printf("1");

        printf("Please enter a message: ");
        printf("2");

        bzero(Client::buffer, 256);
        printf("3");

        fgets(Client::buffer, 255, stdin);
        printf("4");


        n = write(Client::sockfd, Client::buffer, strlen(Client::buffer));
        printf("5");

        if (n < 0)
        {
            perror("Error writing to socket");
        }
        printf("6");
    }

    return nullptr;
}
