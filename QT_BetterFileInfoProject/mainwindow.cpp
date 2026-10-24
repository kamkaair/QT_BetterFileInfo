#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <iostream>
#include <chrono>

void finishingFunc() {
    std::cout << "We are done here" << std::endl;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->folderTree->setAllColumnsShowFocus(false);
    ui->folderTree->setSortingEnabled(true);

    it = new IterateFile;
    it->setTargetTree(ui->folderTree);

    QString defaultPath = "C:/Users/Path";
    ui->textEdit->setText(defaultPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearIterationUI() {
    ui->folderTree->clear();
    it->clearIterations();
    ui->lblTotalSize->setText("0");
}

void MainWindow::on_pushButton_clicked() // Update
{
    // Record starting time
    // Used for benchmarking the tree structure creation. It's currently quite slow.
    auto start = std::chrono::high_resolution_clock::now();

    clearIterationUI();

    std::string pathstd = ui->textEdit->toPlainText().toStdString();
    std::replace(pathstd.begin(), pathstd.end(), '\\', '/');

    it->iteratePath(pathstd);

    QString text2 = it->convertToString(it->convertToDouble(it->getTotalSpaceTaken()));
    ui->lblTotalSize->setText(text2);

    // Record ending time
    auto stop = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto durationS = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    std::cout << "Time taken: " << durationS.count() << " seconds - " << durationMs.count() << " milliseconds" << std::endl;
}

void MainWindow::on_pushButton_2_clicked() // Clear
{
    clearIterationUI();
}

bool createConfirmWindow(const QString& name) {
    QMessageBox msgBox;
    //msgBox.setStyleSheet("background-color: rgb(5, 30, 35);");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle("Deletion");
    msgBox.setText("Delete: " + name + "?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec() == QMessageBox::Yes ? true : false;
}

void MainWindow::on_pushButton_3_clicked() // Delete
{
    QTreeWidgetItem* widgetItem = ui->folderTree->currentItem();
    if(widgetItem == nullptr)
        return;

    if(!createConfirmWindow(widgetItem->text(1)))
        return;

    std::wstring pathwstr = widgetItem->text(1).toStdWString();
    pathwstr.push_back(L'\0'); // zero termination for the string
    const wchar_t* targetPath = pathwstr.c_str();

    if(!it->moveToRecycle(targetPath))
        return;

    std::cout << "Top level item: " << widgetItem->text(1).toStdString() << std::endl;
    it->updateTotalSpaceTaken(ui->lblTotalSize, widgetItem);

    QTreeWidgetItem* widgetParent = widgetItem->parent();
    if(!widgetParent) { // Change the removal method based on whether the item is root or not
        int index = ui->folderTree->currentIndex().row();
        ui->folderTree->takeTopLevelItem(index);
    }
    else {
        widgetParent->removeChild(widgetItem);
    }

    delete widgetItem;
}

