#ifndef USER_SERIAL_H
#define USER_SERIAL_H


#include <QMainWindow>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QByteArray>
#include "common.h"


enum BaudRate {
    Baud1200 = 1200,
    Baud2400 = 2400,
    Baud4800 = 4800,
    Baud9600 = 9600,
    Baud19200 = 19200,
    Baud38400 = 38400,
    Baud57600 = 57600,
    Baud115200 = 115200,
    UnknownBaud = -1
};

enum DataBits {
    Data5 = 5,
    Data6 = 6,
    Data7 = 7,
    Data8 = 8,
    UnknownDataBits = -1
};

enum Parity {
    NoParity = 0,
    EvenParity = 2,
    OddParity = 3,
    SpaceParity = 4,
    MarkParity = 5,
    UnknownParity = -1
};

enum StopBits {
    OneStop = 1,
    OneAndHalfStop = 3,
    TwoStop = 2,
    UnknownStopBits = -1
};


typedef struct User_serial_arg
{
    QString port_name;
    BaudRate baud_rate;
    DataBits data_bit;
    Parity parity;
    StopBits stop_bit;
} User_serial_arg;

class User_serial :public QSerialPort, public QSerialPortInfo
{
    Q_OBJECT
public:
    explicit User_serial(QSerialPort *serial_port_parent = nullptr);
     ~User_serial();

private:
    QStringList available_ports_list;
    User_serial_arg serial_arg;
    QByteArray recv_buf;

public:
    QStringList & user_serial_get_available_ports_name(void);
    int user_serial_open(User_serial_arg arg);
    void user_serial_close(void);
    int user_serial_wirte(const char * send_buf, quint64 buf_len);
    QByteArray & user_serial_read(void);

signals:

};

#endif // USER_SERIAL_H
