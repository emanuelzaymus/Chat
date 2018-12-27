#include "Client.h"

#include "Server.h"

int Client::maxId = 0;
//Server* Client::server = nullptr;

Client::Client(const int socket)
{
    struct clientData* data = new struct clientData();
    dat = data;
    data->socket = socket;
    data->id = maxId++;

    pthread_create(&data->running, NULL, &run, data);
    cout << "SERVER - Client " << data->id << " connected" << endl;
}

Client::~Client()
{
    pthread_join(dat->running, NULL);
    delete dat;
}

void* Client::run(void* ptr)
{
    struct clientData* data = (struct clientData*) ptr;
    int id = data->id;

    pthread_create(&data->reading, NULL, &service, data);

    pthread_join(data->reading, NULL);
    close(data->socket);

    cout << "SERVER - Client " << id << " disconnected" << endl;
    return nullptr;
}

void* Client::service(void* ptr)
{
    struct clientData* data = (struct clientData*) ptr;

    while (/*server->*/Server::isRunning() && data->runningThreads)
    {
        readFromClientWithCheck(data);
    }
    cout << "SERVER - Ended service for client " << data->id << endl;
    return nullptr;
}

void Client::readFromClientWithCheck(struct clientData* data)
{
    readFromClient(data);

    if (strcmp(data->buffer, "end\n") == 0)
    {
        stop(data);
    }
    else if (strcmp(data->buffer, "getFriendsList\n") == 0)
    {
        connectClients(data);
    }
    else if (strcmp(data->buffer, "login\n") == 0)
    {
        login(data);
    }
    else
    {
        sendToClient(data->buffer, data->nick, data->chattingWith->getClientData());
    }
}

void Client::readFromClient(struct clientData* data)
{
    bzero(data->buffer, 256);
    int n = read(data->socket, data->buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
    }
    cout << "Client " << data->id << ": " << data->buffer << flush;
}

void Client::stop(struct clientData* data)
{
    if (data->runningThreads)
    {
        sendToClient(string("SERVER: STOP\n"), data);
        bzero(data->buffer, 256);
        data->runningThreads = false;

        cout << "SERVER - Client " << data->id << " stopped" << endl;
    }
}

void Client::connectClients(struct clientData* data)
{
    do
    {
        sendFriendList(data);
        data->chattingWith = /*server->*/Server::findClientById(readChoice(data));
    }
    while (data->chattingWith == nullptr);
}

void Client::login(struct clientData* data)
{
    readFromClient(data);
    cout << endl;
    strcpy(data->nick, data->buffer);
}

void Client::sendToClient(char msg[256], char fromNick[256], struct clientData* toClient)
{
    if (fromNick != "")
    {
        string finalMsg = string(fromNick) + ": " + string(msg);
        strcpy(msg, finalMsg.c_str());
    }

    int n = write(toClient->socket, msg, strlen(msg) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
    cout << "SERVER - sent msg to client " << toClient->id << endl;
}

void Client::sendToClient(string str, struct clientData* toClient)
{
    bzero(toClient->buffer, 256);
    strcpy(toClient->buffer, str.c_str());
    sendToClient(toClient->buffer, "", toClient);
}

void Client::sendFriendList(struct clientData* data)
{
    bzero(data->buffer, 256);
    strcpy(data->buffer, "You do not have any contacts.\n");
    sendToClient(string(data->buffer), data);
}

int Client::readChoice(struct clientData* data)
{
    readFromClient(data);
    return atoi(data->buffer);
}
