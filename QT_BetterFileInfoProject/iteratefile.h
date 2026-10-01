#ifndef ITERATEFILE_H
#define ITERATEFILE_H

#include <iostream>
#include <filesystem>
#include <string>
#include <functional>
#include <vector>

#include <QTreeWidget>

using namespace std;

struct entryObj {
    uintmax_t size;
    string path;
};

class IterateFile
{
public:
    IterateFile();

    vector<entryObj> iteratePath(string inPath);
    tuple<double, int> convertToDouble(const uintmax_t& fileSize);
    QString convertToString(tuple<double, int> inTuple);

    uintmax_t getTotalSpaceTaken() {return totalSpaceTaken;}
    uintmax_t getTargetFolderSize() {return targetFolderSize;}

private:
    void tryCatch(const string& path, function<void()> func);
    uintmax_t iterateDirectory(const string& path);

    const bool enableWarnigns = false;
    uintmax_t totalSpaceTaken = 0, targetFolderSize = 0;
};

#endif // ITERATEFILE_H
