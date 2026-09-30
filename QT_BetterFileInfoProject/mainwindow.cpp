#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    it = new IterateFile;
    it->iteratePath("C:/Users/Altti/Documents/FantaInMySystem/assets/textures");
}

MainWindow::~MainWindow()
{
    delete ui;
}

