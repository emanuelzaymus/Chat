#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

//namespace ServerSite {

    class Server {
    public:
        Server(char *argv[]);
        Server(const Server& orig);
        virtual ~Server();

    private:
        static int newsockfd;
        static int n;
        static char buffer[256];

        int sockfd;
        socklen_t cli_len;
        struct sockaddr_in serv_addr, cli_addr;

        pthread_t reading;
        pthread_t writing;

        static void* readMsg(void* ptr);
        static void* writeMsg(void* ptr);

    };

//}
#endif /* SERVER_H */

