#ifndef ITERATEFILE_H
#define ITERATEFILE_H

// MOC is causing problems with two of my filesystem includes. These should be at least a temporary fix
// Otherwise I could make a dedicated class with filesystem stuff in it
#ifndef Q_MOC_RUN
#include <filesystem>
#endif

#include <windows.h>

#include <QTreeWidget>
#include <QLabel>
#include <QObject>

#include "structures.h"

using namespace std;

class IterateFile : public QObject // IterateFile now has to inherit QObject, due to the involvement of QObject:connect
{
    Q_OBJECT
public:    
    explicit IterateFile(QObject* parent = nullptr);

    tuple<double, int> convertToDouble(const uintmax_t& fileSize);
    QString convertToString(tuple<double, int> inTuple);

    uintmax_t& getTotalSpaceTaken() {return totalSpaceTaken;}
    void updateTotalSpaceTaken(QLabel* label, QTreeWidgetItem* widgetItem);

    entryObj iterateDirectory(const filesystem::path& path, const std::atomic<bool>& stopExec);
    void createTreeUI(const entryObj& parentEntry, QTreeWidgetItem* parentItem);
    bool moveToRecycle(const wchar_t *const path);
    void setTargetTree(QTreeWidget* widgetTreeRef) { widgetTree = widgetTreeRef;}
    void clearIterations() {
        totalSpaceTaken = 0;
    }

private:
    void tryCatch(const filesystem::path& path, function<void()> func, string err);
    QTreeWidgetItem* addTreeElement(QTreeWidgetItem* treeItem = 0);
    void modifyTreeItem(const entryObj& obj, QTreeWidgetItem* treeItem);
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
