#include "Client.h"

#include "Server.h"

int Client::maxId = 0;

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

struct clientData* Client::getClientData()
{
    return dat;
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

    while (Server::isRunning() && data->runningThreads)
    {
        readWithCheckFrom(data);
    }
    cout << "SERVER - Ended service for client " << data->id << endl;
    return nullptr;
}

void Client::readWithCheckFrom(struct clientData* data)
{
    string msg = readFrom(data);

    //synchronization of reading msgs
    //    cout << "before sending received" << endl;
    //    send("SERVER: received\n", data);
    //    cout << "after sending received" << endl;

    if (msg == "__end\n")
    {
        stop(data);
    }
    else if (msg == "__getFriendsList\n")
    {
        connectWith(data);
    }
    else if (msg == "__logIn\n")
    {
        logIn(data);
    }
    else if (msg == "__signIn\n")
    {
        signIn(data);
    }
    else
    {
        send(data->buffer, data->nick, data->chattingWith->getClientData());
    }
}

string Client::readFrom(struct clientData* data)
{
    bzero(data->buffer, 256);
    int n = read(data->socket, data->buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
    }

    cout << "Client " << data->id << ": " << data->buffer << flush;
    return string(data->buffer);
}

void Client::stop(struct clientData* data)
{
    if (data->runningThreads)
    {
        send(string("SERVER: STOP\n"), data);
        bzero(data->buffer, 256);
        data->runningThreads = false;

        cout << "SERVER - Client " << data->id << " stopped" << endl;
    }
}

void Client::connectWith(struct clientData* data)
{
    do
    {
        sendFriendList(data);
        data->chattingWith = Server::findClientById(readChoice(data));
    }
    while (data->chattingWith == nullptr);
}

void Client::logIn(struct clientData* data)
{
    send("SERVER: tryLogIn\n", data);

    cout << 0 << endl;
    string nick = readFrom(data);
    cout << endl;

    cout << 1 << endl;
    string password = readFrom(data);
    cout << endl;

    cout << 2 << endl;
    // todo: check if is signed in !!!
    // for now is signed in
    send("SERVER: loggedIn\n", data);

    cout << 3 << endl;
    strcpy(data->nick, nick.c_str()); // TODO: make nick sting 
}

void Client::signIn(struct clientData* data)
{
    send("SERVER: trySignIn\n", data);

    cout << 0 << endl;
    string nick = readFrom(data);
    cout << endl;

    cout << 1 << endl;
    string password = readFrom(data);
    cout << endl;

    cout << 2 << endl;
    // todo: check if can be signed in (if nick does not already exists)!!!
    // for now can be signed in
    send("SERVER: loggedIn\n", data);

    cout << 3 << endl;
    strcpy(data->nick, nick.c_str()); // TODO: make nick sting 
}

void Client::send(char msg[256], char fromNick[256], struct clientData* toClient)
{
    addNickToMsg(msg, fromNick);

    int n = write(toClient->socket, msg, strlen(msg) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
    cout << "SERVER - sent msg to client " << toClient->id << ": " << msg << endl;
}

void Client::send(string msg, struct clientData* toClient)
{
    bzero(toClient->buffer, 256);
    strcpy(toClient->buffer, msg.c_str());
    send(toClient->buffer, "", toClient);
}

void Client::sendFriendList(struct clientData* data)
{
    bzero(data->buffer, 256);
    strcpy(data->buffer, "You do not have any contacts.\n");
    send(string(data->buffer), data);
}

int Client::readChoice(struct clientData* data)
{
    //return atoi(readFrom(data));
    readFrom(data);
    return atoi(data->buffer);
}

void Client::addNickToMsg(char msg[256], char fromNick[256])
{
    if (fromNick != "")
    {
        string finalMsg = string(fromNick) + ": " + string(msg);
        strcpy(msg, finalMsg.c_str());
    }
}
