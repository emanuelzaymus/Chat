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

    if (repeated)
    {
        cout << "Nick already exists." << endl;
    }

    do
    {
        clearScreen();
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
        cout << "Wrong nick or password. Try again." << endl;
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
        cout << " 1 - Chat\n"
                << " 2 - Log out\n"
                << " 3 - Delete account\n"
                << " 0 - Exit\n"
                << " Your choice: "
                << flush;
        cin >> choice;
    }
    while (choice < 0 || choice > 2);
    switch (choice)
    {
    case 1: return chatChoice;
    case 2: return logOutChoice;
    case 3: return deleteAccountChoice;
    default: return exitLoggedInMenuChoice;
    }
}

void CLI::clearScreen()
{
    //    system("clear");
}

