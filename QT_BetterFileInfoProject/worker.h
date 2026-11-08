#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include <QThread>
#include <functional>
#include <atomic>

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
    void linkFunction(std::function<entryObj(const std::wstring&, const std::atomic<bool>&)> inFunc, const std::wstring inPath) {
        funcExec = inFunc;
        rootPath = inPath;
    }
    void requestStopExec() { stopExec.store(true); }
    bool getStopExec() { return stopExec.load(std::memory_order_relaxed); }

signals:
    void resultReady(const entryObj &obj);
private:
    std::function<entryObj(const std::wstring&, const std::atomic<bool>&)> funcExec;
    std::wstring rootPath;

    std::atomic<bool> stopExec{false};
};

#endif // WORKER_H
