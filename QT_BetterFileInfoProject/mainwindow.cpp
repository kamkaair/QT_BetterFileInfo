#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    it = new IterateFile;
    it->setTargetTree(ui->folderTree);

    QString defaultPath = "C:/Users/Altti/Downloads/04_GOAP 2/04_GOAP";
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

    std::string pathstd = ui->textEdit->toPlainText().toStdString();
    std::replace(pathstd.begin(), pathstd.end(), '\\', '/');

    //vector<entryObj> entries = it->iteratePath(pathstd);
    it->iteratePath(pathstd);

    /*for (auto& entry : entries) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(ui->folderTree);
        newItem->setText(0, it->convertToString(it->convertToDouble(entry.size)));
        newItem->setText(1, QString::fromStdWString(entry.path));
    }*/

    QString text1 = it->convertToString(it->convertToDouble(it->getTargetFolderSize()));
    QString text2 = it->convertToString(it->convertToDouble(it->getTotalSpaceTaken()));

    ui->lblTargetSize->setText(text1);
    ui->lblTotalSize->setText(text2);
}

void MainWindow::on_pushButton_2_clicked()
{
    clearIterationUI();
}

