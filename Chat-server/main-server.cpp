#include "Server.h"

//using namespace ServerSite;

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return 1;
    }

    Server s(argv);


    return 0;
}
