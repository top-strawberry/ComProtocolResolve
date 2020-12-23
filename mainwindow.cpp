#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , user_serial()
{
    ui->setupUi(this);
    this->setWindowTitle("ComProtocolResolve");
    this->setFixedWidth(1024);
    this->setFixedHeight(800);
    this->mainwindow_update_serial_port();
    connect(&this->user_serial, &QSerialPort::readyRead, this, &MainWindow::mainwindow_readData_slot);
    connect(this->ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::mainwindow_itemDoubleClicked_slot);

    //设置头部的内容
    QStringList list;
    list << "是否有效/描述" << "十六进制/接收数据" << "十六进制/应答数据";
    QTreeWidgetItem *items = new QTreeWidgetItem(list);
    this->ui->treeWidget->setColumnCount(3);
    this->ui->treeWidget->setHeaderItem(items);
    //设置头部 “column” 的长度
    QHeaderView *header = new QHeaderView(Qt::Horizontal);
    header->setDefaultSectionSize(200);
    header->setDefaultAlignment(Qt::AlignLeft);
    this->ui->treeWidget->setHeader(header);
    //添加一个item
    QTreeWidgetItem * item = new QTreeWidgetItem;
    item->setText(0,"测试");
    item->setCheckState(0,Qt::Checked);
    item->setText(1,"01 03 00 00 00 01 84 0A");
    item->setCheckState(1,Qt::Checked);
    item->setText(2,"01 03 02 00 01 79 84");
    item->setCheckState(2,Qt::Checked);
    this->ui->treeWidget->addTopLevelItem(item);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//十六进制字符串转换为字节流
int MainWindow::HexStrToByte(const char *source, char *dest, int sourceLen)
{
    quint8 i,j=0;
    QString c;
    quint8 highByte, lowByte;

    for (i = 0; i < sourceLen; ) {
        if(source[i] == 0x20) {
            i++;
            if(i >= sourceLen){
                qDebug() << "error 0: ";
                return -1;
            }
            continue;
        }
        while(1) {
            if(i >= sourceLen){
                qDebug() << "error 1: ";
                return -1;
            }
            if(source[i] == 0x20) {
                i++;
                continue;
            }
            //highByte = toupper(source[i]);
            c = source[i];
            highByte = c.toUpper().toUtf8().at(0) ;
            if (highByte > 0x39) {
                highByte -= 0x37;
            } else {
                highByte -= 0x30;
            }
            i++;
            break;
        };
        while(1) {
            if(i >= sourceLen){
                qDebug() << "error 2: ";
                return -1;
            }
            if(source[i] == 0x20) {
                i++;
                continue;
            }
            //lowByte  = toupper(source[i]);
            c = source[i];
            lowByte = c.toUpper().toUtf8().at(0);
            if (lowByte > 0x39) {
                lowByte -= 0x37;
            } else {
                lowByte -= 0x30;
            }
            i++;
            break;
        }
        dest[j++] = (highByte << 4) | lowByte;
        highByte = 0;lowByte=0;
    }
    return j;
}

//字节流转换为十六进制字符串
int MainWindow::ByteToHexStr(const unsigned char *source, char *dest, int sourceLen)
{
    short i;
    unsigned char highByte = 0, lowByte = 0;

    for (i = 0; i < sourceLen; i++) {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f;

        highByte += 0x30;
        if (highByte > 0x39) {
            dest[i * 2] = highByte + 0x07;
        } else {
            dest[i * 2] = highByte;
        }

        lowByte += 0x30;
        if (lowByte > 0x39) {
            dest[i * 2 + 1] = lowByte + 0x07;
        } else {
            dest[i * 2 + 1] = lowByte;
        }
    }
    dest[2 * sourceLen] = '\0';
    qDebug() << "2 * sourceLen" << 2 * sourceLen;
    return 2 * sourceLen;
}


User_serial &MainWindow::mainwindow_get_user_serial(void)
{
    return this->user_serial;
}

void MainWindow::mainwindow_set_combobox_enable(bool state)
{
    this->ui->comboBox_serial->setEnabled(state);
    this->ui->comboBox_baud_rate->setEnabled(state);
    this->ui->comboBox_data_bit->setEnabled(state);
    this->ui->comboBox_stop_bit->setEnabled(state);
    this->ui->comboBox_check_bit->setEnabled(state);
    this->ui->button_update->setEnabled(state);
}

void MainWindow::mainwindow_dis_rxd_or_txd(QString dis_type, QByteArray disp_buf)
{
    QString str;
    QDateTime time;

    str = this->ui->textBrowser->toPlainText();
    time = QDateTime::currentDateTime();          //获取系统现在的时间
    str += time.toString("yyyy-MM-dd hh:mm:ss ddd") + dis_type + QString("%1").arg(disp_buf.size()) + "\r\n";         //设置显示格式
    str += tr(disp_buf);
    str = str + "\r\n";
    this->ui->textBrowser->clear();
    this->ui->textBrowser->setText(str);
    this->ui->textBrowser->moveCursor(QTextCursor::End);
}


void MainWindow::mainwindow_update_serial_port(void)
{
    QStringList available_ports_list;

    available_ports_list = this->user_serial.user_serial_get_available_ports_name();
    foreach (QString com, available_ports_list) {
        qDebug() << "available:" + com;
    }
    this->ui->comboBox_serial->clear();
    this->ui->comboBox_serial->addItems(available_ports_list);
}

void MainWindow::on_button_update_clicked()
{
    this->mainwindow_update_serial_port();
}

void MainWindow::on_button_start_clicked()
{
    int ret = 0;
    QString port_name;
    User_serial_arg serial_arg;

    port_name = this->ui->comboBox_serial->currentText();
    serial_arg.port_name = port_name.left(port_name.indexOf("("));
    serial_arg.baud_rate = (BaudRate)this->ui->comboBox_baud_rate->currentText().toUtf8().toInt();
    serial_arg.data_bit = (DataBits)this->ui->comboBox_data_bit->currentText().toUtf8().toInt();
    if(this->ui->comboBox_stop_bit->currentText() == "1"){
        serial_arg.stop_bit = OneStop;
    } else if(this->ui->comboBox_stop_bit->currentText() == "1.5"){
        serial_arg.stop_bit = OneAndHalfStop;
    }else if(this->ui->comboBox_stop_bit->currentText() == "2"){
        serial_arg.stop_bit = TwoStop;
    }
    if(this->ui->comboBox_check_bit->currentText() == "None"){
        serial_arg.parity = NoParity;
    } else if(this->ui->comboBox_stop_bit->currentText() == "Odd"){
        serial_arg.parity = OddParity;
    }else if(this->ui->comboBox_stop_bit->currentText() == "Even"){
        serial_arg.parity = EvenParity;
    }
//    qDebug() << "baud_rate:";
//    qDebug() << serial_arg.baud_rate;
//    qDebug() << "data_bit:";
//    qDebug() << serial_arg.data_bit;
//    qDebug() << "stop_bit:";
//    qDebug() << serial_arg.stop_bit;
//    qDebug() << "parity:";
//    qDebug() << serial_arg.parity;
    ret = this->user_serial.user_serial_open(serial_arg);
    if(ret == 0){
        this->user_serial_isopen = true;
        this->mainwindow_set_combobox_enable(false);
    }
}

void MainWindow::on_button_stop_clicked()
{
    this->user_serial_isopen = false;
    this->user_serial.user_serial_close();
    this->mainwindow_set_combobox_enable(true);
}

void MainWindow::on_button_clear_clicked()
{
    this->ui->textBrowser->clear();
}

void MainWindow::on_button_send_clicked()
{
    int i = 0;
    int item_count = 0;
    QString output_str;
    QTreeWidgetItemIterator it(ui->treeWidget);
    if(this->user_serial_isopen == false){
        return;
    }
    //遍历treeWidget,计算item数量
    while (*it) {
        item_count ++;
        ++it;
    }
    qDebug() << "item_count:" << item_count;
    for (i = 0; i < item_count; i ++) {
        QTreeWidgetItem * item = this->ui->treeWidget->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked){
            output_str = item->text(2);
            qDebug() << output_str.toLatin1().data();
            if(item->checkState(2) == Qt::Checked){//十六进制字符串，转成十六进制再发送
                int data_len = 0;
                QByteArray byteArry = output_str.toLatin1();
                char *dest = (char *)calloc(byteArry.size() + 2, sizeof (char));
                if(dest ==NULL){
                    qDebug() << "error: calloc dest";
                    break;
                }
                qDebug() << "byteArry:" << byteArry << "size:" << byteArry.size();
                data_len = MainWindow::HexStrToByte(output_str.toLatin1().data(), dest, byteArry.size());
                if((data_len > 0)){
                    this->user_serial.user_serial_wirte(dest, data_len);
                }else{
                    QMessageBox::about(NULL, "提示", "输入的应答数据有误!");
                }
                free(dest);
            }else{
                this->user_serial.user_serial_wirte(output_str.toLatin1().data(), output_str.size());
            }
        }
        QThread::sleep(1);
    }
}

void MainWindow::mainwindow_readData_slot()
{
    qDebug() << "*********readData**********";
    int i = 0;
    int item_count = 0;
    QString output_str;
    QByteArray recv_buf;
    QByteArray disp_buf;
    QString str;
    QString textEdit_t_str;
    QByteArray byteArry;
    QTreeWidgetItem * item = NULL;
    int data_len = 0;
    char * dest = NULL;

    bool a_send = false;
    char * send_buf = NULL;
    int send_len = 0;
    bool is_hex = false;


    QTreeWidgetItemIterator it(ui->treeWidget);
    if(this->user_serial_isopen == false){
        return;
    }
    //遍历treeWidget,计算item数量
    while (*it) {
        item_count ++;
        ++it;
    }
    qDebug() << "item_count:" << item_count;
    recv_buf = this->user_serial.user_serial_read();
    qDebug() << "recv_buf:" << recv_buf << "size:" << recv_buf.size();
    for (i = 0; i < item_count; i ++) {
        item = this->ui->treeWidget->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked) {
            byteArry = item->text(1).toLatin1();
            qDebug() << "接收数据:" << byteArry << "size:" << byteArry.size();
            if(item->checkState(1) == Qt::Checked) {//十六进制字符串，转成十六进制再比较
                data_len = 0;
                dest = (char *)calloc(byteArry.size() * 2 + 2, sizeof (char));
                if(dest ==NULL){
                    qDebug() << "error: calloc dest";
                    break;
                }
                //qDebug() << "recv_buf.constData()" << recv_buf.constData();
                //disp_buf = recv_buf.constData();
                data_len = ByteToHexStr((unsigned char*)recv_buf.constData(), dest, recv_buf.size());
                disp_buf = dest;
                qDebug() << "disp_buf.constData()" << disp_buf.constData();
                int len =  disp_buf.length();
                for (int i=2; i<len-1; i+=3,len++) {
                    disp_buf.insert(i," ");
                    //qDebug() << i << str ;
                }
                memset(dest, 0, data_len);

                data_len = MainWindow::HexStrToByte(byteArry.data(), dest, byteArry.size());
                qDebug() << "dest:";
                for (int j= 0; j < data_len; j++) {
                    qDebug() << dest[j];
                }
                qDebug() << "\r\n";

                if((data_len > 0)){
                    if(memcmp(recv_buf, dest, recv_buf.size()) == 0) {
                        if(item->checkState(2) == Qt::Checked){//应答数据使用十六进制发送
                            byteArry.clear();
                            byteArry = item->text(2).toLatin1();
                            memset(dest, 0, data_len);
                            data_len = MainWindow::HexStrToByte(byteArry.data(), dest, byteArry.size());
                            if((data_len > 0)) {
                                byteArry.clear();
                                byteArry = item->text(2).toLatin1();
                                data_len = MainWindow::HexStrToByte(byteArry.data(), dest, byteArry.size());
                                is_hex = a_send = true;
                                send_buf = dest;
                                send_len = data_len;
                            } else {
                                QMessageBox::about(NULL, "提示", "输入的应答数据有误!");
                            }
                        } else {
                            byteArry.clear();
                            byteArry = item->text(2).toLatin1().data();
                            a_send = true;
                            send_buf = byteArry.data();
                            send_len = byteArry.size();
                        }
                        break;
                    }
                } else {
                    QMessageBox::about(NULL, "提示", "输入的接收数据有误!");
                }
            } else {
                disp_buf = recv_buf;
                if(recv_buf == byteArry) {
                    if(item->checkState(2) == Qt::Checked){//应答数据使用十六进制发送
                        byteArry.clear();
                        byteArry = item->text(2).toLatin1();
                        dest = (char *)calloc(byteArry.size() + 2, sizeof (char));
                        if(dest ==NULL){
                            qDebug() << "error: calloc dest";
                            break;
                        }
                        memset(dest, 0, data_len);
                        data_len = MainWindow::HexStrToByte(byteArry.data(), dest, byteArry.size());
                        if((data_len > 0)){
                            is_hex = a_send = true;
                            send_buf = dest;
                            send_len = data_len;
                        }else{
                            QMessageBox::about(NULL, "提示", "输入的应答数据有误!");
                        }
                    }else{
                        byteArry.clear();
                        byteArry = item->text(2).toLatin1();
                        a_send = true;
                        send_buf = byteArry.data();
                        send_len = byteArry.size();
                    }
                    break;
                }
            }
        }
    }
    //显示
    this->mainwindow_dis_rxd_or_txd(" --> rx len:", disp_buf);
    if(a_send == true) {
        char * is_hex_dest = (char *)calloc(send_len * 2 + 2, sizeof (char));
        this->user_serial.user_serial_wirte(send_buf, send_len);
        disp_buf = send_buf;
        if(is_hex == true) {
            if(is_hex_dest == NULL){
                qDebug() << "error: calloc dest";
                return ;
            }
            //qDebug() << "recv_buf.constData()" << recv_buf.constData();
            //disp_buf = recv_buf.constData();
            ByteToHexStr((unsigned char*)send_buf, is_hex_dest, send_len);
            disp_buf = is_hex_dest;
            qDebug() << "disp_buf.constData()" << disp_buf.constData();
            int len =  disp_buf.length();
            for (int i=2; i<len-1; i+=3,len++) {
                disp_buf.insert(i," ");
                //qDebug() << i << str ;
            }
        }
        this->mainwindow_dis_rxd_or_txd(" --> tx len:", disp_buf);
        if(dest)free(dest);
        if(is_hex_dest)free(is_hex_dest);
    }
}


void MainWindow::on_pushButton_add_clicked()
{
    QTreeWidgetItem * item = new QTreeWidgetItem;
    item->setText(0,"");
    item->setCheckState(0,Qt::Unchecked);
    item->setText(1,"");
    item->setCheckState(1,Qt::Unchecked);
    item->setText(2,"");
    item->setCheckState(2,Qt::Unchecked);
    this->ui->treeWidget->addTopLevelItem(item);
}

void MainWindow::on_pushButton_sub_clicked()
{
    int i = 0;
    int item_count = 0;
    QTreeWidgetItemIterator it(ui->treeWidget);
    //遍历treeWidget,计算item数量
    while (*it) {
        item_count ++;
        ++it;
    }
    for (i = 0; i < item_count; i ++) {
        QTreeWidgetItem * item = NULL;
        item = this->ui->treeWidget->topLevelItem(i);
        if(item->isSelected()){
            item = this->ui->treeWidget->takeTopLevelItem(this->ui->treeWidget->currentIndex().row());
            qDebug() << "delete item:" << item;
            delete item;
            item = NULL;
            break;
        }
    }
}

void MainWindow::mainwindow_itemDoubleClicked_slot(QTreeWidgetItem *item, int column)
{
    int ret = -1;
    int  index = -1;
    QString textEdit_d_str;
    QString textEdit_r_str;
    QString textEdit_t_str;
    Ui_user_dialog::User_dialog *user_dialog_ui = NULL;

    column = 0;
    index = this->ui->treeWidget->indexOfTopLevelItem(item);
    qDebug() << "******column:" << column;
    qDebug() << "******index:" << index;
    textEdit_d_str = item->text(0);
    textEdit_r_str = item->text(1);
    textEdit_t_str = item->text(2);
    qDebug() << "******textEdit_d_str:" + textEdit_d_str;
    qDebug() << "******textEdit_r_str:" + textEdit_r_str;
    qDebug() << "******textEdit_t_str:" + textEdit_t_str;

    user_dialog_ui = this->user_dialog.user_dialog_get_ui();
    user_dialog_ui->textEdit_d->setText(textEdit_d_str);
    user_dialog_ui->textEdit_r->setText(textEdit_r_str);
    user_dialog_ui->textEdit_t->setText(textEdit_t_str);

    ret = this->user_dialog.exec();
    if(ret == QDialog::Accepted){//点击确定按钮走这里
        qDebug()<<"accept";
        textEdit_d_str = user_dialog_ui->textEdit_d->toPlainText();
        textEdit_r_str = user_dialog_ui->textEdit_r->toPlainText();
        textEdit_t_str = user_dialog_ui->textEdit_t->toPlainText();
        item->setText(0, textEdit_d_str);
        item->setText(1, textEdit_r_str);
        item->setText(2, textEdit_t_str);
        this->ui->treeWidget->addTopLevelItem(item);
    }else if(ret == QDialog::Rejected){//点击取消按钮走这里
        qDebug()<<"reject";
    }
}


