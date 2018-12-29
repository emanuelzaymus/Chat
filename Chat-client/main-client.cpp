#include "Client.h"

#include "CLI.h"

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        return 1;
    }
    //    string name, pass;
    //    CLI::signIn(name, pass);
    //
    //    cout << name << endl;
    //    cout << pass << endl;


    Client c(argv[1], atoi(argv[2]));

    c.run();


    return 0;
}
