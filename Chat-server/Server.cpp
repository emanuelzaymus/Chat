#include <stddef.h>

#include "Server.h"
#include "FileHandler.h"

const string registeredFilePath = "registered.txt";
const string contactsFile = "_contacts.txt";

int Server::port = -1;
int Server::sockfd = 0;
socklen_t Server::cli_len = 0;
struct sockaddr_in Server::cli_addr = {0};
vector<Client*> Server::clients;
bool Server::running = true;

Server::Server(int port)
{
    Server::port = port;
    bzero((char*) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
    }

    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        perror("Error binding socket address");
    }

    listen(sockfd, 5);
    cli_len = sizeof (cli_addr);
}

Server::~Server()
{
    Client* cl;
    while (clients.size() != 0)
    {
        cl = clients.back();
        clients.pop_back();
        delete cl;
    }
    close(sockfd);
}

void Server::run()
{
    cout << " - - - SERVER STARTED - - - " << endl;

    pthread_create(&consoleReader, NULL, &readConsole, NULL);
    pthread_create(&connecting, NULL, &connectClient, NULL);

    pthread_join(connecting, NULL);
    pthread_join(consoleReader, NULL);

    stopAllClients();

    cout << " - - - SERVER STOPPED - - - " << endl;
}

bool Server::signIn(string nick, string password)
{
    if (isRegistered(nick) || nick == "contacts")
    {
        return false;
    }
    registerClient(nick, password);
    return true;
}

bool Server::logIn(string nick, string password)
{
    return passwordOf(nick) == password;
}

void Server::deleteAccount(string nick)
{
    eraseLineFromFile(nick, registeredFilePath, true);
    vector<string> contacts;
    string path = contactsFilePath(nick);
    FileHandler::readLines(path, contacts);
    for (auto c : contacts)
    {
        removeContact(nick, c);
        remove(getConversationFilePath(nick, c).c_str());
        remove(getConversationFilePath(c, nick).c_str());
    }
    remove(path.c_str());
}

string Server::getContacts(string nick)
{
    string contacts;
    return FileHandler::read(contactsFilePath(nick), contacts) && contacts != ""
            ? contacts
            : "You do not have any contacts.\n";
}

bool Server::addContact(string choseNick, string toNick)
{
    if (isRegistered(choseNick) && !hasContact(choseNick, toNick))
    {
        writeContact(choseNick, toNick);
        writeContact(toNick, choseNick);
        return true;
    }
    return false;
}

bool Server::hasContact(string choseNick, string inNick)
{
    vector<string> contacts;
    if (FileHandler::readLines(contactsFilePath(inNick), contacts))
    {
        for (string contactNick : contacts)
        {
            if (contactNick == choseNick)
            {
                return true;
            }
        }
    }
    return false;
}

bool Server::eraseContact(string choseNick, string fromNick)
{
    return removeContact(choseNick, fromNick) && removeContact(fromNick, choseNick);
}

Client* Server::findClientByNick(string nick)
{
    for (Client* cl : clients)
    {
        if (cl->getClientData()->nick == nick)
        {
            return cl;
        }
    }
    return nullptr;
}

string Server::getConversation(string nick, string chattingWithNick)
{
    return FileHandler::read(getConversationFilePath(nick, chattingWithNick));
}

void Server::writeToConversation(string msg, string nick, string chattingWithNick)
{
    FileHandler::append(getConversationFilePath(nick, chattingWithNick), msg);
}

bool Server::isRunning()
{
    return running;
}

void* Server::connectClient(void* ptr)
{
    int newSockfd;
    while (running)
    {
        newSockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &cli_len);
        if (!running)
        {
            break;
        }
        if (newSockfd < 0)
        {
            perror("ERROR on accept");
        }
        Client* c = new Client(newSockfd);
        clients.push_back(c);
    }
    cout << "SERVER - end of connecting clients" << endl;
    return nullptr;
}

void* Server::readConsole(void* ptr)
{
    string command;
    while (running)
    {
        cin >> command;
        if (command == "stop")
        {
            stopServer();
        }
        else if (command == "read")
        {
            string file;
            cin >> file;
            file += ".txt";
            cout << FileHandler::read(file);
        }
        else if (command == "remove")
        {
            string file;
            cin >> file;
            file += ".txt";
            cout << FileHandler::read(file);
            remove(file.c_str());
        }
    }
    return nullptr;
}

void Server::stopServer()
{
    running = false;
    connectLastClient();
    cout << "SERVER is being stopped..." << endl;
}

void Server::stopAllClients()
{
    for (Client* cl : clients)
    {
        cl->stop(cl->getClientData());
    }
}

void Server::connectLastClient()
{
    struct hostent* server = gethostbyname("frios2.fri.uniza.sk");
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
    }

    struct sockaddr_in serv_addr;

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

void Server::registerClient(string nick, string password)
{
    FileHandler::append(registeredFilePath, nick + "\n" + password + "\n");
}

bool Server::isRegistered(string nick)
{
    return passwordOf(nick) != "";
}

string Server::passwordOf(string nick)
{
    vector<string> registered;
    FileHandler::readLines(registeredFilePath, registered);

    for (int i = 0; i < registered.size(); i += 2)
    {
        if (registered.at(i) == nick)
        {
            return registered.at(i + 1);
        }
    }
    return string("");
}

void Server::writeContact(string choseNick, string toNick)
{
    FileHandler::append(contactsFilePath(toNick), choseNick + "\n");
}

bool Server::removeContact(string choseNick, string fromNick)
{
    return eraseLineFromFile(choseNick, contactsFilePath(fromNick), false);
}

bool Server::eraseLineFromFile(string lineToErase, string path, bool eraseTwoLines)
{
    vector<string> lines;
    if (FileHandler::readLines(path, lines))
    {
        for (int i = 0; i < lines.size(); i++)
        {
            if (lines.at(i) == lineToErase)
            {
                lines.erase(lines.begin() + i);
                if (eraseTwoLines)
                {
                    lines.erase(lines.begin() + i);
                }
                FileHandler::write(path, lines);
                return true;
            }
        }
    }
    return false;
}

string Server::getConversationFilePath(string nick, string chattingWithNick)
{
    //    if (nick > chattingWithNick)
    //    {
    //        nick.swap(chattingWithNick);
    //    }
    return nick.append("_").append(chattingWithNick).append(".txt");
}

string Server::contactsFilePath(string nick)
{
    return nick.append(contactsFile);
}
