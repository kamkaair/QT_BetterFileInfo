#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    it = new IterateFile;
    vector<entryObj> entries = it->iteratePath("C:/Users/Altti/Documents/FantaInMySystem/assets/textures");
    for (auto& entry : entries) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(ui->folderTree);
        newItem->setText(0, tr((entry.path).c_str()));
        newItem->setText(1, it->convertToString(it->convertToDouble(entry.size)));
    }

    QString text1 = it->convertToString(it->convertToDouble(it->getTargetFolderSize()));
    QString text2 = it->convertToString(it->convertToDouble(it->getTotalSpaceTaken()));

    ui->lblTargetSize->setText(text1);
    ui->lblTotalSize->setText(text2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

