#ifndef USER_SERIAL_H
#define USER_SERIAL_H

#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息
#include <QStringList>

class User_serial : public QSerialPort, public QSerialPortInfo
{
    Q_OBJECT
public:
    explicit User_serial(QSerialPort *serial_port_parent = nullptr);
     ~User_serial();

private:
    QStringList available_ports_list;
public:
    //availablePorts
    QStringList & user_serial_get_available_ports_name(User_serial &user_serial);

signals:

};

#endif // USER_SERIAL_H
