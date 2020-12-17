#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ComProtocolResolve");
    this->setFixedWidth(1024);
    this->setFixedHeight(800);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//创建串口对象
//删除串口对象

