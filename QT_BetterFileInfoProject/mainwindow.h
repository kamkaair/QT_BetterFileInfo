#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "iteratefile.h"
#include "spinner.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    void clearIterationUI();
    void execFinished(const entryObj& rootEntry);

    void startChrono();
    void endChrono();

    void lockUI();
    void unlockUI();

    Ui::MainWindow *ui;
    IterateFile* it;

    Spinner* spinner;

    std::chrono::steady_clock::time_point start;
    bool isTimerRunning = false;
};
#endif // MAINWINDOW_H
