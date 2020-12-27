#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , user_serial()
{
    ui->setupUi(this);
    this->setWindowTitle("ComProtocolResolve (by stawberry)");
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
//    QTreeWidgetItem * item = new QTreeWidgetItem;
//    item->setText(0,"测试");
//    item->setCheckState(0,Qt::Checked);
//    item->setText(1,"01 03 00 00 00 01 84 0A");
//    item->setCheckState(1,Qt::Checked);
//    item->setText(2,"01 03 02 00 01 79 84");
//    item->setCheckState(2,Qt::Checked);
//    this->ui->treeWidget->addTopLevelItem(item);
    QString path;
    QDir dir;
    path = kCFG_JSON_ROOT_PATH;
    path += kCFG_JSON_PATH;
    if(!dir.exists(kCFG_JSON_ROOT_PATH)) {
        dir.mkdir(kCFG_JSON_ROOT_PATH);
        QTreeWidgetItem * item = new QTreeWidgetItem;
        item->setText(0,"测试");
        item->setCheckState(0,Qt::Checked);
        item->setText(1,"01 03 00 00 00 01 84 0A");
        item->setCheckState(1,Qt::Checked);
        item->setText(2,"01 03 02 00 01 79 84");
        item->setCheckState(2,Qt::Checked);
        this->ui->treeWidget->addTopLevelItem(item);
    }else {
        this->mainwindow_load_cfg(path);
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

//十六进制字符串转换为字节流
int MainWindow::HexStrToByte(const char *source, char *dest, quint32 sourceLen)
{
    quint32 i,j=0;
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
int MainWindow::ByteToHexStr(const unsigned char *source, char *dest, quint32 sourceLen)
{
    quint32 i;
    quint8 highByte = 0, lowByte = 0;

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

int MainWindow::mainwindow_load_cfg(const QString path)
{
    QByteArray data;
    QJsonDocument doc;
    QJsonArray array;
    QJsonObject obj;
    QJsonValue obj_value;
    int nSize = -1;
    QJsonParseError error;
    User_json_create_object_arg arg;
    QTreeWidgetItem * item = NULL;
    //加载json文件
    QFile file(path);
    if(file.open(QIODevice::ReadOnly) == false){
        return -1;
    }
    data = file.readAll();
    file.close();
    doc = QJsonDocument::fromJson(data, &error);
    if(!doc.isNull() && (QJsonParseError::NoError == error.error)) {
        if (doc.isArray()) { // JSON 文档为数组
            array = doc.array();  // 转化为数组
            nSize = array.size();  // 获取数组大小
            for (int i = 0; i < nSize; ++i) {  // 遍历数组
                obj_value = array.at(i);
                if(obj_value.isObject()) {
                    obj = obj_value.toObject();
                    this->user_json.user_json_parse_object(obj, arg);
                    qDebug() << "describe_checked:" << arg.describe_checked;
                    qDebug() << "rxd_checked:" << arg.rxd_checked;
                    qDebug() << "txd_checked:" << arg.txd_checked;
                    item = new QTreeWidgetItem;
                    item->setCheckState(0, (Qt::CheckState)arg.describe_checked);
                    if(arg.describe_checked == true){
                        item->setCheckState(0, Qt::Checked);
                    }
                    item->setCheckState(1, (Qt::CheckState)arg.rxd_checked);
                    if(arg.rxd_checked == true){
                        item->setCheckState(1, Qt::Checked);
                    }
                    item->setCheckState(2, (Qt::CheckState)arg.txd_checked);
                    if(arg.txd_checked == true){
                        item->setCheckState(2, Qt::Checked);
                    }
                    item->setText(0, arg.describe);
                    item->setText(1, arg.rxd);
                    item->setText(2, arg.txd);
                    this->ui->treeWidget->addTopLevelItem(item);
                }
            }
        }
    }

    return 0;
}

int MainWindow::mainwindow_save_cfg(const QString path)
{
    int i = 0;
    int item_count = 0;
    QByteArray data;
    QJsonDocument doc;
    QJsonArray obj;
    QJsonObject sub;
    User_json_create_object_arg arg;
    QTreeWidgetItem * item = NULL;

    QTreeWidgetItemIterator it(ui->treeWidget);
    //遍历treeWidget,计算item数量
    while (*it) {
        item_count ++;
        ++it;
    }
    qDebug() << "item_count:" << item_count;
    for (i = 0; i < item_count; i ++) {
        item = this->ui->treeWidget->topLevelItem(i);
        arg.describe_checked = item->checkState(0);
        arg.rxd_checked = item->checkState(1);
        arg.txd_checked = item->checkState(2);
        arg.describe = item->text(0);
        arg.rxd = item->text(1);
        arg.txd = item->text(2);
        this->user_json.user_json_create_object(sub, arg);
        qDebug() << "sub:" << sub;
        obj.append(sub);
    }
    doc.setArray(obj);
    data = doc.toJson();

    //json写入文件
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    return 0;
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

    //str = this->ui->textBrowser->toPlainText();
    time = QDateTime::currentDateTime();          //获取系统现在的时间
    str = time.toString("yyyy-MM-dd hh:mm:ss ddd") + dis_type + QString("%1").arg(disp_buf.size()) + "\r\n";         //设置显示格式
    str += tr(disp_buf);
    str = str + "\r\n";
    //this->ui->textBrowser->clear();
    //this->ui->textBrowser->setText(str);
    this->ui->textBrowser->append(str);
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

void MainWindow::mainwindow_delete_QTreeWidgetItem(int currentIndex)
{
    QTreeWidgetItem *item = NULL;
    item = this->ui->treeWidget->takeTopLevelItem(currentIndex);
    qDebug() << "delete item:" << item;
    delete item;
    item = NULL;
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
        if(item->checkState(0) == Qt::Checked) {
            output_str = item->text(2);
            qDebug() << output_str.toLatin1().data();
            if(item->checkState(2) == Qt::Checked) {//十六进制字符串，转成十六进制再发送
                int data_len = 0;
                QByteArray byteArry = output_str.toLatin1();
                char *dest = (char *)calloc(byteArry.size() * 2 + 2, sizeof (char));
                if(dest ==NULL) {
                    qDebug() << "error: calloc dest";
                    break;
                }
                qDebug() << "byteArry:" << byteArry << "size:" << byteArry.size();
                data_len = MainWindow::HexStrToByte(output_str.toLatin1().data(), dest, byteArry.size());
                if((data_len > 0)) {
                    this->user_serial.user_serial_wirte(dest, data_len);
                }else {
                    this->user_messagebox.user_messagebox_about(QString("item %1 您输入的应答数据有误!").arg(i));
                }
                free(dest);
            }else{
                this->user_serial.user_serial_wirte(output_str.toLatin1().data(), output_str.size());
            }
            QThread::sleep(1);
        }
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
            if(byteArry.size() <= 0) {
                continue;
            }
            if(item->checkState(1) == Qt::Checked) {//十六进制字符串，转成十六进制再比较
                data_len = 0;
                dest = (char *)calloc(recv_buf.size() * 2 + 2, sizeof (char));
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

                if((data_len > 0)) {
                    if(memcmp(recv_buf, dest, recv_buf.size()) == 0) {
                        byteArry.clear();
                        byteArry = item->text(2).toLatin1();
                        if(item->checkState(2) == Qt::Checked) {//应答数据使用十六进制发送
                            if(dest) {
                                free(dest);
                                dest = NULL;
                            }
                            dest = (char *)calloc(byteArry.size() * 2 + 2, sizeof (char));
                            if(dest ==NULL) {
                                qDebug() << "error: calloc dest";
                                break;
                            }

                            data_len = MainWindow::HexStrToByte(byteArry.data(), dest, byteArry.size());
                            if((data_len > 0)) {
                                is_hex = a_send = true;
                                send_buf = dest;
                                send_len = data_len;
                            } else {
                                this->user_messagebox.user_messagebox_about(QString("item %1 您输入的应答数据有误!").arg(i));
                            }
                        } else {
                            a_send = true;
                            send_buf = byteArry.data();
                            send_len = byteArry.size();
                        }
                        break;
                    }
                } else {
                    this->user_messagebox.user_messagebox_about(QString("item %1 您输入的接收数据有误!").arg(i));
                }
            } else {
                disp_buf = recv_buf;
                if(recv_buf == byteArry) {
                    byteArry.clear();
                    byteArry = item->text(2).toLatin1();
                    if(item->checkState(2) == Qt::Checked) {//应答数据使用十六进制发送
                        if(dest) {
                            free(dest);
                            dest = NULL;
                        }
                        dest = (char *)calloc(byteArry.size() + 2, sizeof (char));
                        if(dest ==NULL) {
                            qDebug() << "error: calloc dest";
                            break;
                        }

                        data_len = MainWindow::HexStrToByte(byteArry.data(), dest, byteArry.size());
                        if((data_len > 0)) {
                            is_hex = a_send = true;
                            send_buf = dest;
                            send_len = data_len;
                        }else {
                            this->user_messagebox.user_messagebox_about(QString("item %1 您输入的应答数据有误!").arg(i));
                        }
                    }else {
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
            if(is_hex_dest == NULL) {
                qDebug() << "error: calloc dest";
                return ;
            }
            //qDebug() << "recv_buf.constData()" << recv_buf.constData();
            //disp_buf = recv_buf.constData();
            ByteToHexStr((unsigned char*)send_buf, is_hex_dest, send_len);
            disp_buf = is_hex_dest;
            //qDebug() << "应答数据" << disp_buf.constData();
            int len =  disp_buf.length();
            for (int i=2; i<len-1; i+=3,len++) {
                disp_buf.insert(i," ");
                //qDebug() << i << str ;
            }
        }
        this->mainwindow_dis_rxd_or_txd(" --> tx len:", disp_buf);
        if(is_hex_dest)free(is_hex_dest);
    }
    if(dest)free(dest);
}


void MainWindow::on_pushButton_add_clicked()
{
    int item_count = 0;
    QTreeWidgetItemIterator it(ui->treeWidget);
    //遍历treeWidget,计算item数量
    while (*it) {
        item_count ++;
        ++it;
        if(item_count >= 10) {
            this->user_messagebox.user_messagebox_about(QString("配置item,限制总数量不超过10条!"));
            return;
        }
    }

    QTreeWidgetItem * item = new QTreeWidgetItem;
    item->setText(0,"描述说明");
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
            this->mainwindow_delete_QTreeWidgetItem(this->ui->treeWidget->currentIndex().row());
//            item = this->ui->treeWidget->takeTopLevelItem(this->ui->treeWidget->currentIndex().row());
//            qDebug() << "delete item:" << item;
//            delete item;
//            item = NULL;
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
    Ui_user_dialog::User_dialog_ui *user_dialog_ui = NULL;

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

    this->user_dialog.show();
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



void MainWindow::on_action_contact_triggered()
{
    int ret = 0;

    this->user_contact_dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->user_contact_dialog.setWindowFlags(this->windowFlags());
    this->user_contact_dialog.show();
    //ret = this->user_contact_dialog.exec();
    if(ret == QDialog::Accepted){//点击确定按钮走这里
        qDebug()<<"accept";
    }else if(ret == QDialog::Rejected){//点击取消按钮走这里
        qDebug()<<"reject";
    }
}

void MainWindow::on_action_about_triggered()
{
    int ret = 0;

    this->user_about_dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->user_about_dialog.setWindowFlags(this->windowFlags());
    this->user_about_dialog.show();
    //ret = this->user_about_dialog.exec();
    if(ret == QDialog::Accepted){//点击确定按钮走这里
        qDebug()<<"accept";
    }else if(ret == QDialog::Rejected){//点击取消按钮走这里
        qDebug()<<"reject";
    }
}

void MainWindow::on_action_save_triggered()
{
    QString save_path;
    QString default_path;

    save_path = QFileDialog::getSaveFileName(this, tr("保存配置文件"), tr(kCFG_JSON_ROOT_PATH), tr("配置文件(*.json);;所有文件(*);"));
    if(!save_path.isNull()){
        qDebug() << "save_path=" << save_path;
        default_path = kCFG_JSON_ROOT_PATH;
        default_path += kCFG_JSON_PATH;
        if(default_path == save_path) {
            this->mainwindow_save_cfg(default_path);
        } else {
            this->mainwindow_save_cfg(default_path);
            this->mainwindow_save_cfg(save_path);
        }
    }
}

void MainWindow::on_action_open_triggered()
{
//    QStringList str_path_list = QFileDialog::getOpenFileNames(this, tr("选择配置文件"), tr(kCFG_JSON_ROOT_PATH), tr("配置文件(*.json);;"));
//    foreach(QString str_path, str_path_list){
//        qDebug() << "str_path:" << str_path;
//    }
    QString str_path;

    str_path = QFileDialog::getOpenFileName(this, tr("选择配置文件"), tr(kCFG_JSON_ROOT_PATH), tr("配置文件(*.json);;所有文件(*);"));
    if(!str_path.isNull()){
        this->ui->treeWidget->clear();
        qDebug() << "path=" << str_path;
        this->mainwindow_load_cfg(str_path);
    }
}

void MainWindow::on_action_new_triggered()
{
    QString save_path;
    QString default_path;

    save_path = QFileDialog::getSaveFileName(this, tr("保存配置文件"), tr(kCFG_JSON_ROOT_PATH), tr("配置文件(*.json);;所有文件(*);"));
    if(!save_path.isNull()){
        qDebug() << "save_path=" << save_path;
        default_path = kCFG_JSON_ROOT_PATH;
        default_path += kCFG_JSON_PATH;
        if(default_path == save_path) {
            this->mainwindow_save_cfg(default_path);
        } else {
            this->mainwindow_save_cfg(default_path);
            this->mainwindow_save_cfg(save_path);
        }
    }
    this->ui->treeWidget->clear();
}
