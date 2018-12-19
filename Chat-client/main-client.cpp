#include "Client.h"

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        return 1;
    }

    Client c(argv[1], atoi(argv[2]));

    c.run();


    return 0;
}

/**
 * TODO:
 * - Chating between 2 clients (based on IDs)
 *      + NICKs
 * - Login (Registration)
 * - Friend list
 * - Adding friends
 *      + confirmation of friendship
 * - Deleting friends
 *      + callback action on friend site
 * - Saving conversation (on server)
 * - Saving non sent messages
 * 
 * 
 */