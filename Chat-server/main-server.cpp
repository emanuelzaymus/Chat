#include "Server.h"

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return 1;
    }

        Server s(atoi(argv[1]));
    
        s.run();


    return 0;
}
