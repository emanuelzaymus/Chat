#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <pthread.h>

using namespace std;

class FileHandler {
public:
    FileHandler();
    virtual ~FileHandler();

    void append(const string path, const string & text);
    void write(const string path, const vector<string>& vctr);
    string read(const string path);
    bool readLines(const string path, vector<string>& lines);

private:
    pthread_mutex_t mutex;

    void write(const string path, const string & text, bool append);
    bool read(const string path, string& ret);
    string toString(const vector<string>& vctr);

};

#endif /* FILEHANDLER_H */

