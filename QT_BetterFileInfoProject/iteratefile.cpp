#include "iteratefile.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <functional>

IterateFile::IterateFile() {}

void IterateFile::updateTotalSpaceTaken(QLabel* label, QTreeWidgetItem* widgetItem) {
    totalSpaceTaken = totalSpaceTaken - widgetItem->data(0, Qt::UserRole).toULongLong();
    label->setText(convertToString(convertToDouble(totalSpaceTaken)));
}

// General sources for the windows operations:
// https://github.com/reunanen/cpp-move-file-to-trash/blob/master/move-file-to-trash.cpp
// https://www.davekb.com/browse_programming_tips:win32_programmatically_move_file_to_recycle_bin:txt

// Mentions of needing to add a trailing zero (the PCZZSTR string needs to be zero terminated).
// https://stackoverflow.com/questions/42555049/shfileoperation-error-87/42555504#42555504

// Microsoft:
// I guess I could also use IFileOperation, like mentioned in the Microsoft's documention
// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/ns-shellapi-shfileopstructw
// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shfileoperationw
bool IterateFile::moveToRecycle(const wchar_t *const path) {
    //const wchar_t *const outputFile = L"F:/TestFolderDestroy/DestroyThis.txt\0"; // zero termination

    SHFILEOPSTRUCTW fileOperation;
    memset(&fileOperation, 0, sizeof(SHFILEOPSTRUCTW));
    fileOperation.wFunc = FO_DELETE;
    fileOperation.pFrom = path;
    fileOperation.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
    const int result = SHFileOperationW(&fileOperation);

    //if(result != 0) // If the result is not zero, then there's an error
        //cout << "Failed to move to recycling can" << endl;
    return result == 0 ? true : false;
}

QTreeWidgetItem* IterateFile::addTreeElement(QTreeWidgetItem* treeItem) {
    return (treeItem == nullptr) ? new NumericTreeWidgetItem(widgetTree) : new NumericTreeWidgetItem(treeItem);
}

void IterateFile::modifyTreeItem(entryObj& obj, QTreeWidgetItem* treeItem) {
    treeItem->setText(0, convertToString(convertToDouble(obj.size))); // Set the text data
    treeItem->setText(1, QString::fromStdWString(obj.path));

    // Set the original size count for the sortable data. The setTexts above are "cosmetic"
    treeItem->setData(0, Qt::UserRole, QVariant(obj.size));
}

tuple<double, int> IterateFile::convertToDouble(const uintmax_t& fileSize) {
    double mantissa = fileSize;
    int index = 0;

    for (; mantissa >= 1024.0; index++) { // Determine size, index will determine the unit of digital information
        mantissa = mantissa /= 1024.0;
    }
    return make_tuple(mantissa, index);
}

QString IterateFile::convertToString(tuple<double, int> inTuple) {
    const string types = "BKMGTPE";
    double mantissa = get<0>(inTuple);
    int index = get<1>(inTuple);

    string result = to_string(ceil(mantissa * 10.0) / 10.0); // Shift the decimal by one and shift it back after std::ceiling (1.47 -> 14.7 -> ceil(15) -> 1.5)
    result.erase(result.find(".") + 2); // Obliterate the trailing numbers
    result += types[index];
    if (index != 0) {
        result += "B";
    }

    QString resultC_STR = QString::fromStdString(result);
    return resultC_STR;
}

void IterateFile::tryCatch(const filesystem::path& path, function<void()> func, string err) {
    try {
        func();
    }
    catch (const filesystem::filesystem_error& error) {
        if(enableWarnigns)
            cerr << err << path << endl;
    }
}

uintmax_t IterateFile::iterateDirectory(const filesystem::path& path, QTreeWidgetItem* parentItem) {
    uintmax_t totalResult = 0;

    tryCatch(path, [&] {
        filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
        for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {
            tryCatch(path, [&]() {
                if (entry.is_regular_file()) {
                    const uintmax_t fileSize = entry.file_size();
                    totalResult += fileSize;

                    QTreeWidgetItem* newItem = addTreeElement(parentItem);
                    entryObj newEntry = entryObj({ fileSize, entry.path().wstring() });
                    modifyTreeItem(newEntry, newItem);
                }
                else if (entry.is_directory()) {
                    QTreeWidgetItem* newItem = addTreeElement(parentItem);

                    uintmax_t dirTotal = 0;
                    dirTotal += iterateDirectory(entry.path(), newItem); // Later on, if the child dir/files stored, then either store them as wstrings or fs paths!
                    totalResult += dirTotal;

                    entryObj newEntry = entryObj({ dirTotal, entry.path().wstring() });
                    modifyTreeItem(newEntry, newItem);
                }
            }, "SKIPPED: unreadable file: ");
    }}, "SKIPPED: unreadable folder: ");

    return totalResult;
}

void IterateFile::iteratePath(const filesystem::path pathSrc) {
    widgetTree->setSortingEnabled(false);
    totalSpaceTaken += iterateDirectory(pathSrc, 0); // Input 0, so the root items are created
    widgetTree->sortItems(0, Qt::DescendingOrder);
    widgetTree->setSortingEnabled(true);
}
