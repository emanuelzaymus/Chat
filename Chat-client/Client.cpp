#include "Client.h"

const int bufferSize = 2048;

int Client::sockfd = 0;
char Client::bufferIn[bufferSize] = {0};
char Client::bufferOut[bufferSize] = {0};
bool Client::running = true;
bool Client::runningWriting = false;
pthread_t Client::reading;
pthread_t Client::writing;

pthread_mutex_t Client::mutexReading;
pthread_mutex_t Client::mutexWriting;

bool Client::repeatedLogging = false;
string Client::choseNick = "";

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

    pthread_mutex_init(&mutexReading, NULL);
    pthread_mutex_init(&mutexWriting, NULL);
}

Client::~Client()
{
    pthread_mutex_destroy(&mutexReading);
    pthread_mutex_destroy(&mutexWriting);

    close(sockfd);
}

void Client::run()
{
    pthread_create(&reading, NULL, &readMsgs, NULL);

    startMenu();

    pthread_join(reading, NULL);
}

void Client::runWritting()
{
    runningWriting = true;
    pthread_create(&writing, NULL, &writeMsgs, NULL);
}

void Client::stopWritting()
{
    pthread_join(writing, NULL);
}

void* Client::readMsgs(void* ptr)
{
    while (running)
    {
        readFromServerWithCheck();
    }
    cout << "CLIENT - Ended reading service" << endl;
    return nullptr;
}

void* Client::writeMsgs(void* ptr)
{
    string s;
    while (running && runningWriting)
    {
        s = readln();
        writeToServer(s);
    }
    cout << "CLIENT - Ended writing service" << endl;
    return nullptr;
}

void Client::disconnect()
{
    running = false;
    writeToServer("__end");
    cout << "SERVER: Press Enter..." << endl;
    if (runningWriting)
    {
        runningWriting = false;
        stopWritting();
    }
}

void Client::startMenu()
{
    switch (CLI::startMenu())
    {
    case signInChoice:
        signIn();
        break;
    case logInChoice:
        logIn();
        break;
    default:
        disconnect();
        break;
    }
}

void Client::loggedInMenu()
{
    repeatedLogging = false;

    switch (CLI::loggedInMenu())
    {
    case contactsChoice:
        contactsMenu(false);
        break;
    case logOutChoice:
        startMenu();
        break;
    case deleteAccountChoice:
        deleteAccount();
        startMenu();
        break;
    default:
        disconnect();
        break;
    }
}

void Client::deleteAccount()
{
    writeToServer("__deleteAccount");
}

string Client::getContacts()
{
    writeToServer("__getContacts");
    return readFromServer();
}

void Client::contactsMenu(bool wasMistake)
{
    switch (CLI::contactsMenu(getContacts(), choseNick, wasMistake))
    {
    case addContactChoice:
        addContact();
        break;
    case eraseContactChoice:
        eraseContact();
        break;
    case startChatChoice:
        connectInChat();
        break;
    default:
        loggedInMenu();
        break;
    }
}

void Client::addContact()
{
    writeToServer("__addContact");
}

void Client::tryAddContact()
{
    writeToServer(choseNick);
    contactsMenu(readFromServer() != "SERVER: contactAdded");
}

void Client::eraseContact()
{
    writeToServer("__eraseContact");
}

void Client::tryEraseContact()
{
    writeToServer(choseNick);
    contactsMenu(readFromServer() != "SERVER: contactErased");
}

void Client::connectInChat()
{
    writeToServer("__connectInChat");
}

void Client::tryConnectInChat()
{
    writeToServer(choseNick);
    if (readFromServer() == "SERVER: connectedInChat")
    {
        startChat();
    }
    else
    {
        contactsMenu(true);
    }
}

void Client::startChat()
{
    CLI::chat(choseNick, getConversation());
    runWritting();
}

string Client::getConversation()
{
    writeToServer("__getConversation");
    return readFromServer();
}

void Client::tryDisconnectInChat()
{
    stopWritting();
    contactsMenu(false);
}

void Client::signIn()
{
    writeToServer("__signIn");
}

void Client::trySignIn()
{
    string nick;
    string password;

    CLI::signIn(nick, password, repeatedLogging);
    repeatedLogging = true;
    sendNickAndPassword(nick, password);
    if (readFromServer() == "SERVER: loggedIn")
    {
        loggedInMenu();
    }
    else
    {
        signIn();
    }
}

void Client::logIn()
{
    writeToServer("__logIn");
}

void Client::tryLogIn()
{
    string nick;
    string password;

    CLI::logIn(nick, password, repeatedLogging);
    repeatedLogging = true;
    sendNickAndPassword(nick, password);
    if (readFromServer() == "SERVER: loggedIn")
    {
        loggedInMenu();
    }
    else
    {
        logIn();
    }
}

void Client::sendNickAndPassword(string nick, string password)
{
    writeToServer(nick);
    writeToServer(password);
}

void Client::writeToServer()
{
    int n = write(sockfd, bufferOut, strlen(bufferOut));
    if (n < 0)
    {
        perror("Error writing to socket");
    }
}

void Client::writeToServer(string str)
{
    pthread_mutex_lock(&mutexWriting);

    bzero(bufferOut, bufferSize);
    strcpy(bufferOut, str.c_str());
    writeToServer();

    if (str == "__back")
    {
        runningWriting = false;
    }

    pthread_mutex_unlock(&mutexWriting);
}

string Client::readln()
{
    string str;
    getline(cin, str);
    return str;
}

void Client::readFromServerWithCheck()
{
    pthread_mutex_lock(&mutexReading);

    string msgFromServer = readFromServer();

    if (msgFromServer == "SERVER: STOP") disconnect();
    else if (msgFromServer == "SERVER: tryLogIn") tryLogIn();
    else if (msgFromServer == "SERVER: trySignIn") trySignIn();
    else if (msgFromServer == "SERVER: tryAddContact") tryAddContact();
    else if (msgFromServer == "SERVER: tryEraseContact") tryEraseContact();
    else if (msgFromServer == "SERVER: tryConnectInChat") tryConnectInChat();
    else if (msgFromServer == "SERVER: tryDisconnectInChat") tryDisconnectInChat();

    pthread_mutex_unlock(&mutexReading);
}

string Client::readFromServer()
{
    bzero(bufferIn, bufferSize);
    int n = read(sockfd, bufferIn, bufferSize - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
    }
    cout << bufferIn << endl;

    return string(bufferIn);
}

