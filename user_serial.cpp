#include "user_serial.h"
#include "QDebug"

User_serial::User_serial(QSerialPort *serial_port_parent) :
    QSerialPort(serial_port_parent)
{

}

User_serial::~User_serial()
{

}

QStringList &User_serial::user_serial_get_available_ports_name(User_serial &user_serial)
{
    foreach (const QSerialPortInfo & info,  QSerialPortInfo::availablePorts()) {
        user_serial.available_ports_list.append(info.portName());
    }
    qDebug() << user_serial.available_ports_list;

    return user_serial.available_ports_list;
}
