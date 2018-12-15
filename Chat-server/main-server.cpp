#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


int sockfd, newsockfd;
socklen_t cli_len;
struct sockaddr_in serv_addr, cli_addr;
int n;
char buffer[256];

void* readMsg(void* ptr)
{
    while (true)
    {
        printf("1");
        bzero(buffer, 256);
        printf("2");
        n = read(newsockfd, buffer, 255);
        printf("3");

        if (n < 0)
        {
            perror("Error reading from socket");
//            return 4;
        }
        printf("4");
        printf("Here is the message: %s\n", buffer);
        printf("5");
    }

    return nullptr;
}

void* writeMsg(void* ptr)
{
    while (true)
    {
        printf("Please enter a message: ");
        printf("{2}");

        bzero(buffer, 256);
        printf("{3}");

        fgets(buffer, 255, stdin);
        printf("{4}");

        n = write(newsockfd, &buffer, strlen(buffer) + 1);

        if (n < 0)
        {
            perror("Error writing to socket");
//            return 5;
        }
    }

    return nullptr;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5);

    cli_len = sizeof (cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);

    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return 3;
    }

    //--------------------------------------------------------------------------

    pthread_t reading;
    pthread_t writing;

    pthread_create(&reading, NULL, &readMsg, NULL);
    pthread_create(&writing, NULL, &writeMsg, NULL);

    pthread_join(reading, NULL);
    pthread_join(writing, NULL);


    /*
        while (true)
        {
            printf("1");
            bzero(buffer, 256);
            printf("2");
            n = read(newsockfd, buffer, 255);
            printf("3");

            if (n < 0)
            {
                perror("Error reading from socket");
                return 4;
            }
            printf("4");
            printf("Here is the message: %s\n", buffer);
            printf("5");



            printf("Please enter a message: ");
            printf("{2}");

            bzero(buffer, 256);
            printf("{3}");

            fgets(buffer, 255, stdin);
            printf("{4}");

            n = write(newsockfd, &buffer, strlen(buffer) + 1);




            //        const char* msg = "I got your message";
            printf("6");

            //        n = write(newsockfd, msg, strlen(msg) + 1);
            printf("7");


            if (n < 0)
            {
                perror("Error writing to socket");
                return 5;
            }
        }
     */
    //--------------------------------------------------------------------------

    close(newsockfd);
    close(sockfd);

    return 0;
}
