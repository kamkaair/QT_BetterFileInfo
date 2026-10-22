#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include <iostream>
#include <QThread>

class WorkerThread : public QThread
{
    Q_OBJECT
    void run() override {
        QString result;
        /* ... here is the expensive or blocking operation ... */
        for(int i = 0; i < 250; i++) {
            std::cout << "Cool let's test: " << i << std::endl;
        }
        emit resultReady(result);
    }

signals:
    void resultReady(const QString &s);
};

#endif // WORKER_H
