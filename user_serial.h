#ifndef USER_SERIAL_H
#define USER_SERIAL_H

#include <QWidget>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息

class User_serial : public QWidget
{
    Q_OBJECT
public:
    explicit User_serial(QWidget *parent = nullptr);
     ~User_serial();

signals:

};

#endif // USER_SERIAL_H
