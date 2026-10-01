#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    it = new IterateFile;

    QString defaultPath = "C:/Users/Altti/Documents/GrafiikkaMoottori";
    ui->textEdit->setText(defaultPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearIterationUI() {
    ui->folderTree->clear();
    it->clearIterations();
    ui->lblTargetSize->setText("0");
    ui->lblTotalSize->setText("0");
}

void MainWindow::on_pushButton_clicked()
{
    clearIterationUI();

    //C:\Users\Altti\AppData
    //C:/Users/Altti/Documents/MathProgramming/Mine/alttiairaksinen/build
    std::string pathstd = ui->textEdit->toPlainText().toStdString();
    std::replace(pathstd.begin(), pathstd.end(), '\\', '/');
    //QDebug << pathstd;

    vector<entryObj> entries = it->iteratePath(pathstd);
    //vector<entryObj> entries = it->iteratePath("C:/Users/Altti/Documents/MathProgramming/Mine/alttiairaksinen/Submissions");

    for (auto& entry : entries) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(ui->folderTree);
        newItem->setText(0, it->convertToString(it->convertToDouble(entry.size)));
        newItem->setText(1, QString::fromStdWString(entry.path));
    }

    QString text1 = it->convertToString(it->convertToDouble(it->getTargetFolderSize()));
    QString text2 = it->convertToString(it->convertToDouble(it->getTotalSpaceTaken()));

    ui->lblTargetSize->setText(text1);
    ui->lblTotalSize->setText(text2);
}

void MainWindow::on_pushButton_2_clicked()
{
    clearIterationUI();
}

