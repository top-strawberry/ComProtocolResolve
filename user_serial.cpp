#include "user_serial.h"


User_serial::User_serial(QSerialPort *serial_port_parent) :
    QSerialPort(serial_port_parent)
{

}

User_serial::~User_serial()
{

}

QStringList &User_serial::user_serial_get_available_ports_name(void)
{
    QString str;

    this->available_ports_list.clear();
    foreach (const QSerialPortInfo & info,  QSerialPortInfo::availablePorts()) {
        str = info.portName() + "(" + info.description() + ")";
        this->available_ports_list.append(str);
    }
    kLOG_DEBUG() << this->available_ports_list;
    return this->available_ports_list;
}

int User_serial::user_serial_open(User_serial_arg arg)
{
    //设置串口名
    this->setPortName(arg.port_name);
    //设置波特率
    this->setBaudRate(arg.baud_rate);
    //设置数据位数
    switch(arg.data_bit) {
        case 5: this->setDataBits(QSerialPort::Data5); break;
        case 6: this->setDataBits(QSerialPort::Data6); break;
        case 7: this->setDataBits(QSerialPort::Data7); break;
        case 8: this->setDataBits(QSerialPort::Data8); break;
        default: break;
    }
    //设置停止位
    switch(arg.stop_bit) {
        case 1: this->setStopBits(QSerialPort::OneStop); break;
        case 2: this->setStopBits(QSerialPort::TwoStop); break;
        case 3: this->setStopBits(QSerialPort::OneAndHalfStop); break;
        default: break;
    }
    //设置奇偶校验
    switch(arg.parity) {
        case 0: this->setParity(QSerialPort::NoParity); break;
        case 2: this->setParity(QSerialPort::EvenParity); break;
        case 3: this->setParity(QSerialPort::OddParity); break;
        default: break;
    }
    this->setFlowControl(QSerialPort::NoFlowControl);

    //打开串口
    if(!this->open(QIODevice::ReadWrite)) {
        QMessageBox::about(NULL, "提示", "无法打开" + arg.port_name);
        return -1;
    }
    return 0;
}

void User_serial::user_serial_close()
{
    if (this->isOpen()) this->close();
}

int User_serial::user_serial_wirte(const char * send_buf, quint64 buf_len)
{
//    kLOG_DEBUG() << "user serial open ok";
//    kLOG_DEBUG() << "baud_rate:";
//    kLOG_DEBUG() << this->baudRate();
//    kLOG_DEBUG() << "data_bit:";
//    kLOG_DEBUG() << this->dataBits();
//    kLOG_DEBUG() << "stop_bit:";
//    kLOG_DEBUG() << this->stopBits();
//    kLOG_DEBUG() << "parity:";
//    kLOG_DEBUG() << this->parity();
    this->write(send_buf, buf_len);
    return 0;
}

QByteArray & User_serial::user_serial_read()
{
    recv_buf = this->readAll();
    if (!recv_buf.isEmpty()) {
        kLOG_DEBUG() << recv_buf;
    }
    return recv_buf;
}





















