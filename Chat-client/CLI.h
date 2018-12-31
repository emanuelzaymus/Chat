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
    contactsChoice,
    logOutChoice,
    deleteAccountChoice
};

enum ContactsMenuChoice {
    bakcContactsMenuChoice,
    addContactChoice,
    eraseContactChoice,
    startChatChoice
};

class CLI {
public:
    CLI();
    virtual ~CLI();

    static StartMenuChoice startMenu();
    static void signIn(string& nick, string& password, bool repeated);
    static void logIn(string& nick, string& password, bool repeated);
    static LoggedInMenuChoice loggedInMenu();
    static ContactsMenuChoice contactsMenu(string contacts, string& choseNick, bool wasMistake);
    static void chat(string withNick, string msgs);

private:
    static void clearScreen();

};

#endif /* CLI_H */

