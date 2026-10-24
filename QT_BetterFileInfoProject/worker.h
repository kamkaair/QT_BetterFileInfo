#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include <iostream>
#include <QThread>
#include <functional>

// MOC is causing problems with two of my filesystem includes. These should be at least a temporary fix
// Otherwise I could make a dedicated class with filesystem stuff in it
#ifndef Q_MOC_RUN
#include <filesystem>
#endif

#include "structures.h"

class WorkerThread : public QThread
{
    //entryObj IterateFile::iterateDirectory(const filesystem::path& path)
    Q_OBJECT
    void run() override {
        if(!funcExec)
            return;

        entryObj result = funcExec(rootPath);
        emit resultReady(result); // Emit the resulting rootEntry to the finishing method
    }

public:
    void linkFunction(std::function<entryObj(const std::filesystem::path&)> inFunc, const std::filesystem::path inPath) {
        funcExec = inFunc;
        rootPath = inPath;
    }

signals:
    //void resultReady(const QString &s);
    void resultReady(const entryObj &obj);
private:
    std::function<entryObj(const std::filesystem::path&)> funcExec;
    std::filesystem::path rootPath;
};

#endif // WORKER_H
