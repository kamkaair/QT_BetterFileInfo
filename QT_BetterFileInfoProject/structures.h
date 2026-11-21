#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <vector>
#include <string>

struct entryObj {
    unsigned long long int size = 0;
    std::wstring fileName;

    std::vector<entryObj> children;
};

#endif // STRUCTURES_H
