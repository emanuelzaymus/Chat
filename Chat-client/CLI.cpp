#include <stdexcept>

#include "CLI.h"

CLI::CLI()
{
}

CLI::~CLI()
{
}

StartMenuChoice CLI::startMenu()
{
    int choice;
    do
    {
        clearScreen();
        cout << " 1 - Sign in\n"
                << " 2 - Log in\n"
                << " 0 - Exit\n"
                << " Your choice: "
                << flush;
        cin >> choice;
    }
    while (choice < 0 || choice > 2);
    switch (choice)
    {
    case 1: return signInChoice;
    case 2: return logInChoice;
    default: return exitStartMenuChoice;
    }
}

void CLI::signIn(string& nick, string& password, bool repeated)
{
    bool first = true;
    string pass2;

    do
    {
        clearScreen();
        if (repeated)
        {
            cout << "Nick already exists! Try again." << endl;
        }
        if (!first)
        {
            cout << "Passwords are not equal!" << endl;
        }
        cout << "Nick: " << flush;
        if (first)
        {
            cin >> nick;
            first = false;
        }
        else
        {
            cout << nick << endl;
        }
        cout << "Password 1x: " << flush;
        cin >> password;
        cout << "Password 2x: " << flush;
        cin >> pass2;
    }
    while (password != pass2);
}

void CLI::logIn(string& nick, string& password, bool repeated)
{
    clearScreen();
    if (repeated)
    {
        cout << "Wrong nick or password! Try again." << endl;
    }

    cout << "Nick: " << flush;
    cin >> nick;
    cout << "Password: " << flush;
    cin >> password;
}

LoggedInMenuChoice CLI::loggedInMenu()
{
    int choice;
    do
    {
        clearScreen();
        cout << " 1 - Contacts\n"
                << " 2 - Log out\n"
                << " 3 - Delete account\n"
                << " 0 - Exit\n"
                << " Your choice: "
                << flush;
        cin >> choice;
    }
    while (choice < 0 || choice > 3);
    switch (choice)
    {
    case 1: return contactsChoice;
    case 2: return logOutChoice;
    case 3: return deleteAccountChoice;
    default: return exitLoggedInMenuChoice;
    }
}

ContactsMenuChoice CLI::contactsMenu(string contacts, string& choseNick, bool wasMistake)
{
    int choice;
    do
    {
        clearScreen();
        if (wasMistake)
        {
            cout << "Unknown contact! Try again." << endl;
        }

        cout << "Your contacts:\n" << contacts << endl;

        cout << " 1 - Add contact\n"
                << " 2 - Erase contact\n"
                << " 3 - Start chat\n"
                << " 0 - Back\n"
                << " Your choice: "
                << flush;
        cin >> choice;
        if (choice >= 1 && choice <= 3)
        {
            cout << "Enter nick: " << flush;
            cin >> choseNick;
        }
    }
    while (choice < 0
           || choice > 3
           || choseNick == "" && choice >= 1 && choice <= 3);
    switch (choice)
    {
    case 1: return addContactChoice;
    case 2: return eraseContactChoice;
    case 3: return startChatChoice;
    default: return bakcContactsMenuChoice;
    }
}

void CLI::chat(string withNick, string msgs)
{
    clearScreen();
    cout << "To end chat type: \"__back\"" << endl;
    cout << "CHAT with: " << withNick << endl << endl;
    cout << msgs << flush;
}

void CLI::clearScreen()
{
    system("clear");
}

