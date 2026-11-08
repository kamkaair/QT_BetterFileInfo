#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <vector>
#include <string>

struct entryObj {
    //uintmax_t size = 0;
    unsigned long long int size = 0;
    std::wstring path;

    std::vector<entryObj> children;
};

#endif // STRUCTURES_H
