#include "Client.h"

int Client::sockfd = 0;
int Client::n = 0;
char Client::buffer[256] = {0};

bool Client::running = true;

pthread_t Client::reading;
pthread_t Client::writing;

pthread_mutex_t Client::mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Client::cond = PTHREAD_COND_INITIALIZER;

Client::Client(const char* hostName, int port)
{
    server = gethostbyname(hostName);
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
    serv_addr.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
    }

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        perror("Error connecting to socket");
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&reading, NULL, &readMsg, NULL);
    pthread_create(&writing, NULL, &writeMsg, NULL);
}

Client::~Client()
{
    //    std::cout << "in client destructor - start" << std::endl;
    pthread_join(reading, NULL);
    pthread_join(writing, NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    close(sockfd);
    //    std::cout << "in client destructor - end" << std::endl;
}

void* Client::readMsg(void* ptr)
{
    while (running)
    {
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);

        if (n < 0)
        {
            perror("Error reading from socket");
        }

        std::cout << "Server: " << buffer;
    }
    std::cout << "end of readMsg method" << std::endl;
    return nullptr;
}

void* Client::writeMsg(void* ptr)
{
    while (running)
    {
        std::cout << "You: ";

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        if (strcmp(buffer, "end\n") == 0)
        {
            disconnect();
        }
        n = write(sockfd, buffer, strlen(buffer));

        if (n < 0)
        {
            perror("Error writing to socket");
        }
    }
    std::cout << "end of writeMsg method" << std::endl;
    return nullptr;
}

void Client::disconnect()
{
    running = false;
}