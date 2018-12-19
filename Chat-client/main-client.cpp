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
