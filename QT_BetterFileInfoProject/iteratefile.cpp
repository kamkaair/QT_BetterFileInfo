#include "iteratefile.h"
#include <cmath>
#include <algorithm>
#include <QDebug>
#include <QTreeWidgetItem>

IterateFile::IterateFile()
{

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

uintmax_t IterateFile::iterateDirectory(const filesystem::path& path) {
    uintmax_t totalResult = 0;

    tryCatch(path, [&] {
            filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
            for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {
                tryCatch(path, [&]() {
                    if (entry.is_regular_file()) {
                        totalResult += entry.file_size();
                    }
                    else if (entry.is_directory()) {
                        totalResult += iterateDirectory(entry.path()); // Later on, if the child dir/files stored, then either store them as wstrings or fs paths!
                    }
                }, "SKIPPED: unreadable file: ");
            }}, "SKIPPED: unreadable folder: ");

    return totalResult;
}

bool sortEntries(entryObj const& lhs, entryObj const& rhs) {
    return lhs.size > rhs.size;
}

vector<entryObj> IterateFile::iteratePath(const filesystem::path pathSrc) {
    vector<entryObj> resultsVec;

    tryCatch(pathSrc, [&] {
            filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
            for (filesystem::directory_entry const& entry : filesystem::directory_iterator(pathSrc, settings)) {
                tryCatch(pathSrc, [&] {
                    if (entry.is_regular_file()) {
                        targetFolderSize += entry.file_size();
                    }
                    else if (entry.is_directory()) {
                        uintmax_t dirTotal = iterateDirectory(entry.path());
                        totalSpaceTaken += dirTotal;
                        resultsVec.push_back(entryObj({ dirTotal, entry.path().wstring() })); // Used wstring, since some paths might have dubious characters. Should be able to handle those (test case had chinese letters)
                    }
                }, "SKIPPED: unreadable file: ");
            }
        }, "SKIPPED: unreadable folder: ");

    sort(resultsVec.begin(), resultsVec.end(), sortEntries); // Sort the list of folder sizes from the largest to smallest

    return resultsVec;
}
