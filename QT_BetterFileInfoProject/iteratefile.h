#ifndef ITERATEFILE_H
#define ITERATEFILE_H

#include <iostream>
#include <filesystem>
#include <string>
#include <functional>
#include <vector>

using namespace std;

struct entryObj {
    uintmax_t size;
    string path;
};

class IterateFile
{
public:
    IterateFile();

    void iteratePath(string inPath);

private:
    tuple<double, int> convertToDouble(const uintmax_t& fileSize);
    string convertToString(tuple<double, int> inTuple);
    void tryCatch(const string& path, function<void()> func);
    uintmax_t iterateDirectory(const string& path);

    const bool enableWarnigns = false;
};

#endif // ITERATEFILE_H
