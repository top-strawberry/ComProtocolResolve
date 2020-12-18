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
    this->mainwindow_update_serial_port();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::mainwindow_update_serial_port()
{
    QStringList available_ports_list;
    available_ports_list = this->user_serial.user_serial_get_available_ports_name(this->user_serial);
    this->ui->comboBox_serial->addItems(available_ports_list);
    foreach (QString com, available_ports_list) {
        qDebug() << "available:" + com;
    }
}

//创建串口对象
//删除串口对象

