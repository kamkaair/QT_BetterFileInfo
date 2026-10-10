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

// Great source for sorting in qt. Used some of the code for defining my own sorting logic:
// https://runebook.dev/en/docs/qt/qtreewidgetitem/sortChildren
class NumericTreeWidgetItem : public QTreeWidgetItem {
public:
    // Explicit constructors for QTreeWidget and QTreeWidgetItem, both are used either as root or childs
    explicit NumericTreeWidgetItem(QTreeWidget* parent) : QTreeWidgetItem(parent) {}
    explicit NumericTreeWidgetItem(QTreeWidgetItem* child) : QTreeWidgetItem(child) {}
    // Could also use this, it should achieve the same as explicitly typing the constructors:
    // using QTreeWidgetItem::QTreeWidgetItem;

    bool operator<(const QTreeWidgetItem& other) const override {
        // Comparison between the current QTreeWidgetItem data and the target item
        return data(0, Qt::UserRole).toDouble() < other.data(0, Qt::UserRole).toDouble();
    }
};

#endif // ITERATEFILE_H
