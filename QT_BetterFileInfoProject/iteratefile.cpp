#include "iteratefile.h"

#include <cmath>
#include <algorithm>

#include <iostream>
#include <string>

IterateFile::IterateFile(QObject* parent) : QObject(parent) {}

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

QTreeWidgetItem* IterateFile::addTreeElement(QTreeWidgetItem* treeItem) { // Parent or not
    return (treeItem == nullptr) ? new NumericTreeWidgetItem(widgetTree) : new NumericTreeWidgetItem(treeItem);
}

void IterateFile::modifyTreeItem(const entryObj& obj, QTreeWidgetItem* treeItem) {
    treeItem->setText(0, convertToString(convertToDouble(obj.size))); // Set the text data
    treeItem->setText(1, QString::fromStdWString(obj.fileName));

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

/*entryObj IterateFile::iterateDirectory(const filesystem::path& path, const std::atomic<bool>& stopExec) {
    uintmax_t totalResult = 0;
    entryObj parentEntry;
    parentEntry.path = path;

    if(stopExec) // Possibly break, before entering the loop
        return parentEntry;

    tryCatch(path, [&] {
        filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied;
        for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {
            if(stopExec) // Halt the iteration upon user's command
               break;

            tryCatch(path, [&]() {
                if (entry.is_regular_file()) {
                    const uintmax_t fileSize = entry.file_size();
                    totalResult += fileSize;

                    entryObj newEntry = entryObj({ fileSize, entry.path().wstring() });
                    parentEntry.children.push_back(newEntry);

                }
                else if (entry.is_directory()) {
                    entryObj newEntry = iterateDirectory(entry.path(), stopExec); // Later on, if the child dir/files stored, then either store them as wstrings or fs paths!
                    totalResult += newEntry.size;
                    parentEntry.children.push_back(newEntry);
                }
            }, "SKIPPED: unreadable file: ");
    }}, "SKIPPED: unreadable folder: ");
    parentEntry.size = totalResult;

    return parentEntry;
}*/

void IterateFile::createEntry(entryObj&& newItem, entryObj& parentEntry) {
    // https://learn.microsoft.com/en-us/previous-versions/ms915521(v=msdn.10)
    // (nFileSizeHigh * MAXDWORD+1) + nFileSizeLow
    parentEntry.size += newItem.size;
    parentEntry.children.push_back(std::move(newItem)); // Move newItem, instead of copying. Ts was taking a lot of memory
}

entryObj IterateFile::iterateDirectoriesWindows(const wstring& path, const std::atomic<bool>& stopExec, const wstring& mask) {
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;

    entryObj parentEntry;
    parentEntry.fileName = path;
    wstring spec = path + L"\\" + mask;

    if(stopExec.load()) // Possibly break, before entering the loop
        return parentEntry;

    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfileexw
    hFind = FindFirstFileExW(spec.c_str(), FindExInfoBasic, &ffd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    //hFind = FindFirstFile(spec.c_str(), &ffd);

    if (hFind == INVALID_HANDLE_VALUE)  {
        //wcout << "INVALID: " << spec << endl;
        return parentEntry;
    }

    do {
        //if (ffd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            //continue; // zero-size entry

        if(stopExec) // Halt the iteration upon user's command
           break;
        if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0)
            continue;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            createEntry(iterateDirectoriesWindows(path + L"\\" + ffd.cFileName, stopExec, mask), parentEntry);
            totalFolders++;
        }
        else {
            // The multiplication can happen using 32-bit arithmetic before the result is assigned to the 64-bit variable
            unsigned long long int fileSize = (static_cast<unsigned long long>(ffd.nFileSizeHigh) << 32) | ffd.nFileSizeLow;
            //createEntry(entryObj({ fileSize, path + L"\\" + ffd.cFileName }), parentEntry);
            parentEntry.size += fileSize;
            totalFiles++;
        }

    } while (FindNextFile(hFind, &ffd) != 0);
    FindClose(hFind);

    // Testing
    totalEntryMemory += sizeof(parentEntry);

    return parentEntry;
}

void IterateFile::iterateDirectoryWindows(const wstring& path, QTreeWidgetItem* parentItem, const wstring& mask) {
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    wstring spec = path + L"\\" + mask;

    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfileexw
    hFind = FindFirstFileExW(spec.c_str(), FindExInfoBasic, &ffd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0)
            continue;
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            totalFolders++;
            continue;
        }
        // The multiplication can happen using 32-bit arithmetic before the result is assigned to the 64-bit variable
        unsigned long long int fileSize = (static_cast<unsigned long long>(ffd.nFileSizeHigh) << 32) | ffd.nFileSizeLow;
        entryObj newEntry = { fileSize, path + L"\\" + ffd.cFileName };

        QTreeWidgetItem* newItem = addTreeElement(parentItem);
        modifyTreeItem(newEntry, newItem);

        totalFiles++;
    } while (FindNextFile(hFind, &ffd) != 0);
    FindClose(hFind);
}

void IterateFile::createTreeUI(const entryObj& parentEntry, QTreeWidgetItem* parentItem) {
    QTreeWidgetItem* newItem = addTreeElement(parentItem);
    modifyTreeItem(parentEntry, newItem);

    newItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator); // Able to collapse trees, even if empty

    for(auto& childEntry : parentEntry.children)
        createTreeUI(childEntry, newItem);
}
