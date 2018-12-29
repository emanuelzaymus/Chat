#ifndef CLI_H
#define CLI_H

#include <iostream>

using namespace std;

enum StartMenuChoice {
    exitStartMenuChoice,
    signInChoice,
    logInChoice
};

enum LoggedInMenuChoice {
    exitLoggedInMenuChoice,
    chatChoice,
    logOutChoice,
    deleteAccountChoice
};

class CLI {
public:
    CLI();
    virtual ~CLI();

    static StartMenuChoice startMenu();
    static void signIn(string& nick, string& password, bool repeated);
    static void logIn(string& nick, string& password, bool repeated);
    static LoggedInMenuChoice loggedInMenu();

private:
    static void clearScreen();

};

#endif /* CLI_H */

