#include "FileHandler.h"

void FileHandler::append(const string path, const string& text)
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

void FileHandler::write(const string path, const vector<string>& vctr)
{
    string text = toString(vctr);
    write(path, text);
}

bool FileHandler::read(const string path, string& ret)
{
    ifstream inFile(path);
    if (!inFile.is_open())
    {
        cerr << "Cannot read file!!!" << endl;
        return false;
    }

    getline(inFile, ret, (char) inFile.eof());
    inFile.close();

    return true;
}

string FileHandler::read(const string path)
{
    string ret;
    return read(path, ret) ? ret : "";
}

bool FileHandler::readLines(const string path, vector<string>& lines)
{
    ifstream inFile(path);

    if (!inFile)
    {
        cerr << "Cannot open the file " << path << endl;
        return false;
    }

    string str;
    while (getline(inFile, str))
        //    while (inFile >> str)
    {
        if (str.size() > 0)
        {
            lines.push_back(str);
        }
    }

    inFile.close();
    return true;
}

string FileHandler::toString(const vector<string>& vctr)
{
    string ret = "";
    for (auto line : vctr)
    {
        ret.append(line).append("\n");
    }
    return ret;
}

