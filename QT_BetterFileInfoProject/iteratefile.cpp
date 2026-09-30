#include "iteratefile.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

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

string IterateFile::convertToString(tuple<double, int> inTuple) {
    const string types = "BKMGTPE";
    double mantissa = get<0>(inTuple);
    int index = get<1>(inTuple);

    string result = to_string(ceil(mantissa * 10.0) / 10.0); // Shift the decimal by one and shift it back after std::ceiling (1.47 -> 14.7 -> ceil(15) -> 1.5)
    result.erase(result.find(".") + 2); // Obliterate the trailing numbers
    result += types[index];
    if (index != 0) {
        result += "B";
    }

    return result;
}

void IterateFile::tryCatch(const string& path, function<void()> func) {
    try {
        func();
    }
    catch (const filesystem::filesystem_error& error) {
        if(enableWarnigns)
            cerr << "SKIPPED: unreadable file: " << path << endl;
    }
}

uintmax_t IterateFile::iterateDirectory(const string& path) {
    uintmax_t totalResult = 0;

    filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
    for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {
        tryCatch(path, [&]() {
            if (entry.is_regular_file()) {
                totalResult += entry.file_size();
            }
            else if (entry.is_directory()) {
                totalResult += iterateDirectory(entry.path().string());
            }
        });
    }

    return totalResult;
}

bool sortEntries(entryObj const& lhs, entryObj const& rhs) {
    return lhs.size > rhs.size;
}

void IterateFile::iteratePath(string inPath) {
    string path = inPath;
    uintmax_t totalSpaceTaken = 0, targetFolderSize = 0;
    vector<entryObj> resultsVec;

    filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
    for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {
        tryCatch(path, [&] {
            if (entry.is_regular_file()) {
                targetFolderSize += entry.file_size();
            }
            else if (entry.is_directory()) {
                uintmax_t dirTotal = iterateDirectory(entry.path().string());
                totalSpaceTaken += dirTotal;
                resultsVec.push_back(entryObj({ dirTotal, entry.path().string() }));
            }
        });
    }

    sort(resultsVec.begin(), resultsVec.end(), sortEntries); // Sort the list of folder sizes from the largest to smallest

    for (auto& entry : resultsVec)
        qDebug() << "C++ Style Debug Message";
        //cout << entry.path << " - " << convertToString(convertToDouble(entry.size)) << endl;

    string targetFolderSizeStr = convertToString(convertToDouble(targetFolderSize));
    string totalUsedSpaceStr = convertToString(convertToDouble(totalSpaceTaken));
    //cout << endl;
    //cout << "Used space in the target folder: " << convertToString(convertToDouble(targetFolderSize)) << endl;
    //cout << "Total used space in " << path << " - " << convertToString(convertToDouble(totalSpaceTaken)) << endl;
}
