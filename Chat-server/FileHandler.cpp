#include "FileHandler.h"

void FileHandler::writeAppend(const string path, const string& text)
{
    ofstream outFile(path, ofstream::app);
    outFile << text;
    outFile.close();
}

void FileHandler::write(const string path, const string& text)
{
    ofstream outFile(path);
    outFile << text;
    outFile.close();
}

string FileHandler::read(const string path)
{
    ifstream inFile(path);
    if (!inFile.is_open())
    {
        cout << "Cannot read file!!!" << endl;
    }

    string ret;
    getline(inFile, ret, (char) inFile.eof());
    inFile.close();

    return ret;
}

bool FileHandler::read(const string path, vector<string>& vecOfStrs)
{
    ifstream inFile(path);

    if (!inFile)
    {
        cerr << "Cannot open the file " << path << endl;
        return false;
    }

    string str;
//        while (getline(inFile, str))
    while (inFile >> str)
    {
        if (str.size() > 0)
            vecOfStrs.push_back(str);
    }

    inFile.close();
    return true;
}