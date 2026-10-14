#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

void MainWindow::on_pushButton_clicked()
{
    clearIterationUI();

    std::string pathstd = ui->textEdit->toPlainText().toStdString();
    std::replace(pathstd.begin(), pathstd.end(), '\\', '/');

    it->iteratePath(pathstd);

    QString text2 = it->convertToString(it->convertToDouble(it->getTotalSpaceTaken()));
    ui->lblTotalSize->setText(text2);
}

void MainWindow::on_pushButton_2_clicked()
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

void MainWindow::on_pushButton_3_clicked()
{
    //int column = ui->folderTree->currentColumn();
    QTreeWidgetItem* widgetItem = ui->folderTree->currentItem();
    if(!createConfirmWindow(widgetItem->text(1)))
        return;

    if(widgetItem == nullptr)
        return;

    QTreeWidgetItem* widgetParent = widgetItem->parent();
    if(widgetParent == nullptr) {
        int index = ui->folderTree->currentIndex().row();
        int childCount = widgetItem->childCount();
        if(!childCount == 0) {
            // TODO: recursively delete all of the pointers of children folders and files
            for(int i = 0; i < childCount; i++) {
                QTreeWidgetItem* child = widgetItem->child(i);
                std::cout << child->text(1).toStdString() << std::endl;
                widgetItem->removeChild(child);
                delete child;
            }
        }
        ui->folderTree->takeTopLevelItem(index);
        std::cout << "Top level item: " << widgetItem->text(1).toStdString() << std::endl;
        delete widgetItem;
        return;
    }

    std::cout << "Widget: " << widgetItem->text(1).toStdString() << std::endl;

    widgetParent->removeChild(widgetItem);
    delete widgetItem;
}

