#ifndef ITERATEFILE_H
#define ITERATEFILE_H

#include <filesystem>
#include <string>
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

    void iteratePath(const filesystem::path pathSrc);
    tuple<double, int> convertToDouble(const uintmax_t& fileSize);
    QString convertToString(tuple<double, int> inTuple);

    uintmax_t getTotalSpaceTaken() {return totalSpaceTaken;}
    void setTargetTree(QTreeWidget* widgetTreeRef) { widgetTree = widgetTreeRef;}
    void clearIterations() {
        totalSpaceTaken = 0;
    }

private:
    void tryCatch(const filesystem::path& path, function<void()> func, string err);
    uintmax_t iterateDirectory(const filesystem::path& path, QTreeWidgetItem* parentItem);

    QTreeWidgetItem* addTreeElement(QTreeWidgetItem* treeItem = 0);
    void modifyTreeItem(entryObj& obj, QTreeWidgetItem* treeItem);
    QTreeWidget* widgetTree;

    const bool enableWarnigns = false;
    uintmax_t totalSpaceTaken = 0;
};

#endif // ITERATEFILE_H
