#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include <QThread>
#include <functional>
#include <atomic>

// MOC is causing problems with two of my filesystem includes. These should be at least a temporary fix
// Otherwise I could make a dedicated class with filesystem stuff in it
#ifndef Q_MOC_RUN
#include <filesystem>
#endif

#include "structures.h"

class WorkerThread : public QThread
{
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
    /*void requestStopExec() { stopExec.store(true); }
    void resetExec() { stopExec.store(false); }
    bool getStopExec() { return stopExec.load(); }*/

signals:
    void resultReady(const entryObj &obj);
private:
    //std::function<entryObj(const std::filesystem::path&, WorkerThread*)> funcExec;
    std::function<entryObj(const std::filesystem::path&)> funcExec;
    std::filesystem::path rootPath;

    //std::atomic<bool> stopExec{false};
};

#endif // WORKER_H
