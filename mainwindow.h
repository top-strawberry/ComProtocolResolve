#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QString>
#include <QThread>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QThread>
#include "common.h"
#include <iostream>
#include "user_dialog.h"
#include "user_contact_dialog.h"
#include "user_about_dialog.h"
#include "user_baud_rate_dialog.h"
#include "user_messagebox.h"


#define kCFG_JSON_ROOT_PATH             "config/"
#define kCFG_JSON_PATH                  "cfg.json"
#define  kDIS_TYPE_TX                   " --> tx len:"
#define  kDIS_TYPE_RX                   " --> rx len:"
#define  kQTreeWidgetItemIteratorSize   20

typedef int (*cfg_opt)(const QString path);

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include "user_serial.h"
#include "user_json.h"

class MainWindow;

class Worker:public QObject
{
    Q_OBJECT
public:
    Worker(){};
    ~Worker(){};
public slots:
    void mainwindow_readDataDeleteLater(Worker * self);
    // doWork定义了线程要执行的操作
    void mainwindow_readDataDoworkSlot(MainWindow * mainWindow);

// 线程完成工作时发送的信号
signals:
     void readDataResultReadySignal(const unsigned long long threadId);// 线程完成工作时发送的信号
};


typedef struct {
    unsigned long long id;
    Worker *worker;
    QThread *readDataThread;
} stReadDataThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:  //声明静态成员函数
    static int HexStrToByte(const char *source, char *dest, quint32 sourceLen);
    static int ByteArrayToHexStr(const unsigned char *source, char *dest, quint32 sourceLen);
    static QString byteArrayToHexStr(const QByteArray &data);

private:
    Ui::MainWindow *ui;
public:
    bool user_serial_isopen;

    stReadDataThread readDataThreadTable[256];
    User_serial user_serial;
    User_json user_json;
    User_dialog user_dialog;
    User_contact_dialog user_contact_dialog;
    User_about_dialog user_about_dialog;
    User_baud_rate_dialog user_baud_dialog;
    User_messagebox user_messagebox; 
    QByteArray recv_buf;
    bool isShow;                //是否显示数据
    bool isHexShow;
    bool isAutoClear;

public:
    User_serial & mainwindow_get_user_serial(void);
    int mainwindow_save_cfg(const QString path);
    int mainwindow_load_cfg(const QString path);
    void mainwindow_set_combobox_enable(bool state);
    void mainwindow_item_double_clicked(QTreeWidgetItem *item, int column);
    void mainwindow_dis_rxd_or_txd(QString dis_type, QByteArray disp_buf);
    void mainwindow_update_serial_port(void);
    void mainwindow_delete_QTreeWidgetItem(int currentIndex);
    void mainwindow_add_test_item(void);
    void mainwindow_readDataThreadTable_get(stReadDataThread *arr[256]);
    Ui::MainWindow * mainwindow_ui_get(void);
    bool mainwindow_user_serial_isopen_get(void);
    int mainwindow_itemCount_get(void);
    void mainwindow_data_doWork(QByteArray & byteData);

private slots:
    void mainwindow_itemDoubleClicked_slot(QTreeWidgetItem *item, int column);
    void mainwindow_qcombobox_activated_slot(int index);
    void on_button_update_clicked();
    void on_button_start_clicked();
    void on_button_stop_clicked();
    void on_button_clear_clicked();
    void on_button_send_clicked();
    void mainwindow_readData_slot();
    void on_pushButton_add_clicked();
    void on_pushButton_sub_clicked();
    void on_action_contact_triggered();
    void on_action_about_triggered();
    void on_action_save_triggered();
    void on_action_open_triggered();
    void on_action_new_triggered();
    void on_chStopDis_clicked(bool checked);

    // 处理线程执行的结果
    void handleResults(const unsigned long long threadId);

signals:
    void readDataOperate(MainWindow * self); // 发送信号触发线程
};
#endif // MAINWINDOW_H
