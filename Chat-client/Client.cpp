#include "Client.h"

int Client::sockfd = 0;
char Client::buffer[256] = {0};
bool Client::running = true;
pthread_t Client::reading;
pthread_t Client::writing;

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
}

Client::~Client()
{
    close(sockfd);
}

void Client::run()
{
    login();
    getFriendsList();

    pthread_create(&reading, NULL, &readMsgs, NULL);
    pthread_create(&writing, NULL, &writeMsgs, NULL);

    pthread_join(reading, NULL);
    pthread_join(writing, NULL);
}

void* Client::readMsgs(void* ptr)
{
    while (running)
    {
        readFromServer();
    }
    cout << "CLIENT - Ended reading service" << endl;
    return nullptr;
}

void* Client::writeMsgs(void* ptr)
{
    string s;
    while (running)
    {
        s = readln();
        s.append("\n\0");
        writeToServer(s);
    }
    cout << "CLIENT - Ended writing service" << endl;
    return nullptr;
}

void Client::disconnect()
{
    running = false;
}

void Client::getFriendsList()
{
    cout << "Make choice:" << endl;
    writeToServer("getFriendsList\n");
    readFromServer();
}

void Client::login()
{
    writeToServer("login\n");
    cout << "Login: " << flush;
    writeToServer(readln());
    sleep(1);
}

void Client::writeToServer()
{
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
    }

    if (strcmp(buffer, "end\n") == 0)
    {
        disconnect();
    }
}

void Client::writeToServer(string str)
{
    bzero(buffer, 256);
    strcpy(buffer, str.c_str());
    writeToServer();
}

string Client::readln()
{
    string str;
    getline(cin, str);
    return str;
}

void Client::readFromServer()
{
    bzero(buffer, 256);
    int n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
    }
    cout << buffer << flush;

    if (strcmp(buffer, "SERVER: STOP\n") == 0 && running)
    {
        disconnect();
        cout << "SERVER: Press Enter..." << endl;
    }
}