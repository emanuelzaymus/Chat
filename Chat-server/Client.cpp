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
    else if (msg == "__logIn\n")
    {
        logIn(data);
    }
    else if (msg == "__signIn\n")
    {
        signIn(data);
    }
    else if (msg == "__getContacts\n")
    {
        getContacts(data);
    }
    else if (msg == "__addContact\n")
    {
        addContact(data);
    }
    else if (msg == "__eraseContact\n")
    {
        eraseContact(data);
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

void Client::logIn(struct clientData* data)
{
    send("SERVER: tryLogIn\n", data);

    string nick;
    string password;
    readNickAndPassword(nick, password, data);

    if (Server::logIn(nick, password))
    {
        data->nick = nick;
        send("SERVER: loggedIn\n", data);
    }
    else
    {
        send("SERVER: NOT loggedIn\n", data);
    }
}

void Client::signIn(struct clientData* data)
{
    send("SERVER: trySignIn\n", data);

    string nick;
    string password;
    readNickAndPassword(nick, password, data);

    if (Server::signIn(nick, password))
    {
        data->nick = nick;
        send("SERVER: loggedIn\n", data);
    }
    else
    {
        send("SERVER: NOT loggedIn\n", data);
    }
}

void Client::getContacts(struct clientData* data)
{
//    send("SERVER: readContacts\n", data);
//    readFrom(data); // sync
    send(Server::getContacts(data->nick), data);
}

void Client::addContact(struct clientData* data)
{
    send("SERVER: tryAddContact\n", data);
    string choseNick = readFrom(data);
    cout << endl;
    if (Server::addContact(choseNick, data->nick))
    {
        send("SERVER: contactAdded\n", data);
    }
    else
    {
        send("SERVER: contact was not added\n", data);
    }
}

void Client::eraseContact(struct clientData* data)
{
    send("SERVER: tryEraseContact\n", data);
    string choseNick = readFrom(data);
    cout << endl;
    if (Server::eraseContact(choseNick, data->nick))
    {
        send("SERVER: contactErased\n", data);
    }
    else
    {
        send("SERVER: contact was not erased\n", data);
    }
}

void Client::send(char msg[256], string fromNick, struct clientData* toClient)
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

void Client::readNickAndPassword(string& nick, string& password, struct clientData* data)
{
    nick = readFrom(data);
    cout << endl;
    password = readFrom(data);
    cout << endl;
}

void Client::addNickToMsg(char msg[256], string fromNick)
{
    if (fromNick != "")
    {
        string finalMsg = fromNick + ": " + string(msg);
        strcpy(msg, finalMsg.c_str());
    }
}
