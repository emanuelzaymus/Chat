#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class FileHandler {
public:
    static void append(const string path, const string & text);
    static void write(const string path, const string & text);
    static string read(const string path);
    static bool readLines(const string path, vector<string>& lines);

};

#endif /* FILEHANDLER_H */

