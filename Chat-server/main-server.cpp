#include "Server.h"

#include "FileHandler.h"

#include <vector>
#include <string>

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return 1;
    }
    /*
    //        FileHandler::writeAppend("registered.txt", "peto 111\n");
            FileHandler::write("registered.txt", "");

        //    cout << FileHandler::read("registered.txt") << endl;

        vector<string> vec;

        if (FileHandler::read("registered.txt", vec))
        {
            for (auto elem : vec)
            {
                cout << "<" << elem << ">" << endl;
            }

        }
     */


    Server s(atoi(argv[1]));

    s.run();

    return 0;
}
