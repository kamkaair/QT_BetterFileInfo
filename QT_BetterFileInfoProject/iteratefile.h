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
    wstring path;
};

class IterateFile
{
public:
    IterateFile();

    vector<entryObj> iteratePath(const filesystem::path pathSrc);
    tuple<double, int> convertToDouble(const uintmax_t& fileSize);
    QString convertToString(tuple<double, int> inTuple);

    uintmax_t getTotalSpaceTaken() {return totalSpaceTaken;}
    uintmax_t getTargetFolderSize() {return targetFolderSize;}

private:
    void tryCatch(const filesystem::path& path, function<void()> func, string err);
    uintmax_t iterateDirectory(const filesystem::path& path);

    const bool enableWarnigns = false;
    uintmax_t totalSpaceTaken = 0, targetFolderSize = 0;
};

#endif // ITERATEFILE_H
