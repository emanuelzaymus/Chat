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


int sockfd, n;
struct sockaddr_in serv_addr;
struct hostent* server;

char buffer[256];

pthread_mutex_t mutex;

void* readMsg(void* ptr)
{
    while (true)
    {
        bzero(buffer, 256);
        printf("7");

        n = read(sockfd, buffer, 255);
        printf("8");
        printf("Here is the message: %s\n", buffer);
        printf("9");

        if (n < 0)
        {
            perror("Error reading from socket");
//            return 6;
        }
    }

    return nullptr;
}

void* writeMsg(void* ptr)
{
    while (true)
    {
        printf("1");

        printf("Please enter a message: ");
        printf("2");

        bzero(buffer, 256);
        printf("3");

        fgets(buffer, 255, stdin);
        printf("4");


        n = write(sockfd, buffer, strlen(buffer));
        printf("5");

        if (n < 0)
        {
            perror("Error writing to socket");
//            return 5;
        }
        printf("6");
    }

    return nullptr;
}

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
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
        return 3;
    }

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }

    //--------------------------------------------------------------------------

    pthread_mutex_init(&mutex, NULL);

    pthread_t reading;
    pthread_t writing;

    pthread_create(&reading, NULL, &readMsg, NULL);
    pthread_create(&writing, NULL, &writeMsg, NULL);

    pthread_join(reading, NULL);
    pthread_join(writing, NULL);

    pthread_mutex_destroy(&mutex);

    /*
        while (true)
        {
            printf("1");

            printf("Please enter a message: ");
            printf("2");

            bzero(buffer, 256);
            printf("3");

            fgets(buffer, 255, stdin);
            printf("4");


            n = write(sockfd, buffer, strlen(buffer));
            printf("5");

            if (n < 0)
            {
                perror("Error writing to socket");
                return 5;
            }
            printf("6");

            bzero(buffer, 256);
            printf("7");

            n = read(sockfd, buffer, 255);
            printf("8");
            printf("Here is the message: %s\n", buffer);
            printf("9");

            if (n < 0)
            {
                perror("Error reading from socket");
                return 6;
            }
        }
     */
    //--------------------------------------------------------------------------

    printf("%s\n", buffer);
    close(sockfd);

    return 0;
}