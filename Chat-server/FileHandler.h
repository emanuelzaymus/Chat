#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class FileHandler {
public:
    static void writeAppend(const string path, const string & text);
    static void write(const string path, const string & text);
    static string read(const string path);
    static bool read(const string path, vector<string>& vecOfStrs);

};

#endif /* FILEHANDLER_H */

