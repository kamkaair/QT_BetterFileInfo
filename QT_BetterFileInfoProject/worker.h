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
        entryObj result = funcExec(rootPath, stopExec);

        if(!stopExec) {
            emit resultReady(result); // Emit the resulting rootEntry to the finishing method
        }
    }

public:
    void linkFunction(std::function<entryObj(const std::filesystem::path&, const std::atomic<bool>&)> inFunc, const std::filesystem::path inPath) {
        funcExec = inFunc;
        rootPath = inPath;
    }
    void requestStopExec() { stopExec.store(true); }
    bool getStopExec() { return stopExec.load(std::memory_order_relaxed); }

signals:
    void resultReady(const entryObj &obj);
private:
    std::function<entryObj(const std::filesystem::path&, const std::atomic<bool>&)> funcExec;
    std::filesystem::path rootPath;

    std::atomic<bool> stopExec{false};
};

#endif // WORKER_H
