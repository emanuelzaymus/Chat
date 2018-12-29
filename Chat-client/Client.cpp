#include "Client.h"

int Client::sockfd = 0;
char Client::buffer[256] = {0};
bool Client::running = true;
pthread_t Client::reading;
pthread_t Client::writing;

pthread_mutex_t Client::mutex;
pthread_cond_t Client::cond;
bool Client::isLocked = false;
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
}

Client::~Client()
{
    pthread_join(reading, NULL); // maybie at the and of run()
    pthread_join(writing, NULL); // maybie at the and of run()
    close(sockfd);
}

void Client::run()
{
    pthread_create(&reading, NULL, &readMsgs, NULL); //todo maybie runReading()

    startMenu();
}

void Client::runWritting() // todo call somewhere !!!
{
    pthread_create(&writing, NULL, &writeMsgs, NULL);
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
    writeToServer("__end\n");
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
        getContacts(false);
        break;
    case logOutChoice:
        startMenu();
        break;
    case deleteAccountChoice:
        //        deleteAccount();
        startMenu();
        break;
    default:
        disconnect();
        break;
    }
}

void Client::getContacts(bool wasMistake)
{
    writeToServer("__getContacts\n");
    contactsMenu(readFromServer(), wasMistake);
}

void Client::contactsMenu(string contacts, bool wasMistake)
{
    switch (CLI::contactsMenu(contacts, choseNick, wasMistake))
    {
    case addContactChoice:
        addContact();
        break;
    case eraseContactChoice:
        eraseContact();
        break;
    case startChatChoice:
        //        chat(choseNick);
        break;
    default:
        loggedInMenu();
        break;
    }
}

void Client::addContact()
{
    writeToServer("__addContact\n");
}

void Client::tryAddContact()
{
    writeToServer(choseNick);
    getContacts(readFromServer() != "SERVER: contactAdded\n");
}

void Client::eraseContact()
{
    writeToServer("__eraseContact\n");
}

void Client::tryEraseContact()
{
    writeToServer(choseNick);
    getContacts(readFromServer() != "SERVER: contactErased\n");
}

void Client::signIn()
{
    writeToServer("__signIn\n");
}

void Client::trySignIn()
{
    string nick;
    string password;

    CLI::signIn(nick, password, repeatedLogging);
    repeatedLogging = true;
    sendNickAndPassword(nick, password);
    if (readFromServer() == "SERVER: loggedIn\n")
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
    writeToServer("__logIn\n");
}

void Client::tryLogIn()
{
    string nick;
    string password;

    CLI::logIn(nick, password, repeatedLogging);
    repeatedLogging = true;
    sendNickAndPassword(nick, password);
    if (readFromServer() == "SERVER: loggedIn\n")
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
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
    }
}

void Client::writeToServer(string str)
{
    /*pthread_mutex_lock(&mutex);
    if (isLocked)
    {
        cout << "   writing waiting" << endl;
        pthread_cond_wait(&cond, &mutex);
//        readFromServerWithCheck(); !!!!
    }
    isLocked = true;*/

    //    cout << "client - writing: " << endl;

    bzero(buffer, 256);
    strcpy(buffer, str.c_str());
    writeToServer();

    //    cout << "client - end writing: " << endl;

    //        isLocked = false;
    //    pthread_mutex_unlock(&mutex);
    //    pthread_cond_signal(&cond);  !!!
}

string Client::readln()
{
    string str;
    getline(cin, str);
    return str;
}

void Client::readFromServerWithCheck()
{
    //        pthread_mutex_lock(&mutex);
    //        if (isLocked)
    //        {
    //            cout << "   reading waiting" << endl;
    //            pthread_cond_wait(&cond, &mutex);
    //        }
    //        isLocked = true;

    //    cout << "client reading: " << endl;

    string msgFromServer = readFromServer();
    if (running) // todo: is it necessary????
    {
        if (msgFromServer == "SERVER: STOP\n")
        {
            disconnect();
            cout << "SERVER: Press Enter..." << endl;
        }
            //        else if (msgFromServer == "SERVER: received\n") // todo IS NOT NECESSARY NOW !!!
            //        {
            //        cout << "in readWithCheck - if received" << endl;
            //        isLocked = false;
            //        pthread_cond_signal(&cond);
            //        }
        else if (msgFromServer == "SERVER: tryLogIn\n")
        {
            tryLogIn();
        }
        else if (msgFromServer == "SERVER: trySignIn\n")
        {
            trySignIn();
        }
        else if (msgFromServer == "SERVER: tryAddContact\n")
        {
            tryAddContact();
        }
        else if (msgFromServer == "SERVER: tryEraseContact\n")
        {
            tryEraseContact();
        }
    }

    //    cout << "client end reading: " << endl;

    //        isLocked = false;
    //        pthread_mutex_unlock(&mutex);
    //        pthread_cond_signal(&cond);
}

string Client::readFromServer()
{
    bzero(buffer, 256);
    int n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
    }
    cout << buffer << flush;

    return string(buffer);
}

