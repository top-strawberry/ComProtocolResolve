#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QString>
#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <iostream>
#include "user_dialog.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include "user_serial.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:  //声明静态成员函数
    static int HexStrToByte(const char *source, char *dest, quint32 sourceLen);
    static int ByteToHexStr(const unsigned char *source, char *dest, quint32 sourceLen);

private:
    Ui::MainWindow *ui;
private:
    bool user_serial_isopen;
    User_serial user_serial;
    User_dialog user_dialog;
    void mainwindow_update_serial_port(void);
public:
    User_serial & mainwindow_get_user_serial(void);
    void mainwindow_set_combobox_enable(bool state);
    void mainwindow_item_double_clicked(QTreeWidgetItem *item, int column);
    void mainwindow_dis_rxd_or_txd(QString dis_type, QByteArray disp_buf);

private slots:
    void on_button_update_clicked();
    void on_button_start_clicked();
    void on_button_stop_clicked();
    void on_button_clear_clicked();
    void on_button_send_clicked();
    void mainwindow_readData_slot();
    void on_pushButton_add_clicked();
    void mainwindow_itemDoubleClicked_slot(QTreeWidgetItem *item, int column);
    void on_pushButton_sub_clicked();
};
#endif // MAINWINDOW_H
