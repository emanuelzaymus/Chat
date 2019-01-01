#include "Client.h"

#include "Server.h"

const int bufferSize = 2048;

int Client::maxId = 0;

Client::Client(const int socket)
{
    struct clientData* data = new struct clientData();
    clientData = data;
    data->socket = socket;
    data->id = maxId++;

    pthread_create(&data->running, NULL, &run, data);
    cout << "SERVER - Client " << data->id << " connected" << endl;
}

Client::~Client()
{
    pthread_join(clientData->running, NULL);
    delete clientData;
}

struct clientData* Client::getClientData()
{
    return clientData;
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

    int id = data->id;
    while (Server::isRunning() && data->runningThreads)
    {
        readWithCheckFrom(data);
    }
    cout << "SERVER - Ended service for client " << id << endl;
    return nullptr;
}

void Client::readWithCheckFrom(struct clientData* data)
{
    string msg = readFrom(data);

    if (msg == "__end") stop(data);
    else if (msg == "__signIn") signIn(data);
    else if (msg == "__logIn") logIn(data);
    else if (msg == "__deleteAccount") deleteAccount(data);
    else if (msg == "__getContacts") getContacts(data);
    else if (msg == "__addContact") addContact(data);
    else if (msg == "__eraseContact") eraseContact(data);
    else if (msg == "__connectInChat") connectInChat(data);
    else if (msg == "__getConversation") getConversation(data);
    else if (msg == "__back") disconnectInChat(data);
    else deliver(data->buffer, data, data->chattingWithNick);
}

string Client::readFrom(struct clientData* data)
{
    bzero(data->buffer, bufferSize);
    int n = read(data->socket, data->buffer, bufferSize - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
    }

    cout << "Client " << data->id << ": " << data->buffer << endl;
    return string(data->buffer);
}

void Client::stop(struct clientData* data)
{
    if (data->runningThreads)
    {
        int id = data->id;
        send(string("SERVER: STOP"), data);
        bzero(data->buffer, bufferSize);
        data->runningThreads = false;
        Server::stopClient(data->nick);

        cout << "SERVER - Client " << id << " stopped" << endl;
    }
}

void Client::signIn(struct clientData* data)
{
    send("SERVER: trySignIn", data);

    string nick = readFrom(data);
    string password = readFrom(data);

    if (Server::signIn(nick, password))
    {
        data->nick = nick;
        send("SERVER: loggedIn", data);
    }
    else
    {
        send("SERVER: NOT loggedIn", data);
    }
}

void Client::logIn(struct clientData* data)
{
    send("SERVER: tryLogIn", data);

    string nick = readFrom(data);
    string password = readFrom(data);

    if (Server::logIn(nick, password))
    {
        data->nick = nick;
        send("SERVER: loggedIn", data);
    }
    else
    {
        send("SERVER: NOT loggedIn", data);
    }
}

void Client::deleteAccount(struct clientData* data)
{
    Server::deleteAccount(data->nick);
}

void Client::getContacts(struct clientData* data)
{
    send(Server::getContacts(data->nick), data);
}

void Client::addContact(struct clientData* data)
{
    send("SERVER: tryAddContact", data);
    string choseNick = readFrom(data);
    if (choseNick != data->nick && Server::addContact(choseNick, data->nick))
    {
        send("SERVER: contactAdded", data);
    }
    else
    {
        send("SERVER: contact was not added", data);
    }
}

void Client::eraseContact(struct clientData* data)
{
    send("SERVER: tryEraseContact", data);
    string choseNick = readFrom(data);
    if (Server::eraseContact(choseNick, data->nick))
    {
        send("SERVER: contactErased", data);
    }
    else
    {
        send("SERVER: contact was not erased", data);
    }
}

void Client::connectInChat(struct clientData* data)
{
    send("SERVER: tryConnectInChat", data);
    string choseNick = readFrom(data);
    if (Server::hasContact(choseNick, data->nick))
    {
        data->chattingWithNick = choseNick;
        send("SERVER: connectedInChat", data);
    }
    else
    {
        send("SERVER: contact was not erased", data);
    }
}

void Client::getConversation(struct clientData* data)
{
    send(Server::getConversation(data->nick, data->chattingWithNick), data);
}

void Client::disconnectInChat(struct clientData* data)
{
    send("SERVER: tryDisconnectInChat", data);
    data->chattingWithNick = "";
}

void Client::deliver(char msg[bufferSize], struct clientData* fromClient, string toNick)
{
    Server::writeToConversation(string(msg) + "\n", fromClient->nick, toNick);
    addNickToMsg(msg, fromClient->nick);
    Server::writeToConversation(string(msg) + "\n", toNick, fromClient->nick);

    Client* cl = Server::findClientByNick(toNick);
    if (cl != nullptr && cl->clientData->chattingWithNick == fromClient->nick)
    {
        send(msg, cl->clientData);
    }
}

void Client::send(char msg[bufferSize], struct clientData* toClient)
{
    int n = write(toClient->socket, msg, strlen(msg) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
    cout << "SERVER - sent msg to client " << toClient->id << ": " << msg << endl;
}

void Client::send(string msg, struct clientData* toClient)
{
    bzero(toClient->buffer, bufferSize);
    strcpy(toClient->buffer, msg.c_str());
    send(toClient->buffer, toClient);
}

void Client::addNickToMsg(char msg[bufferSize], string fromNick)
{
    if (fromNick != "")
    {
        string finalMsg = fromNick + ": " + string(msg);
        strcpy(msg, finalMsg.c_str());
    }
}

