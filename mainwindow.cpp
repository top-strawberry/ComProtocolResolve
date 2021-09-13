#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QHeaderView>

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
    connect(this->ui->comboBox_baud_rate,SIGNAL(activated(int)), this, SLOT(mainwindow_qcombobox_activated_slot(int)));

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
        this->mainwindow_add_test_item();
    }else {
        QFile file(path);
        if(!file.exists()) {
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            file.close();
            this->mainwindow_add_test_item();
        }
        this->mainwindow_load_cfg(path);
    }

    this->isShow = false;
    memset(this->readDataThreadTable, 0, ARRAY_SIZE(this->readDataThreadTable) * sizeof(stReadDataThread));
}


MainWindow::~MainWindow()
{
    delete ui;
    unsigned long long tableId = 0;
    for (tableId = 0; tableId < ARRAY_SIZE(readDataThreadTable); tableId++) {
        readDataThreadTable[tableId].readDataThread->quit();
        readDataThreadTable[tableId].readDataThread->wait();
        readDataThreadTable[tableId].id = 0;
        delete readDataThreadTable[tableId].readDataThread;
        readDataThreadTable[tableId].readDataThread = nullptr;
    }
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
                kLOG_DEBUG() << "error 0: ";
                return -1;
            }
            continue;
        }
        while(1) {
            if(i >= sourceLen){
                kLOG_DEBUG() << "error 1: ";
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
                kLOG_DEBUG() << "error 2: ";
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
int MainWindow::ByteArrayToHexStr(const unsigned char *source, char *dest, quint32 sourceLen)
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
    kLOG_DEBUG() << "2 * sourceLen" << 2 * sourceLen;
    return 2 * sourceLen;
}

QString MainWindow::byteArrayToHexStr(const QByteArray &data)
{
    QString temp = "";
    QString hex = data.toHex();

    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }

#if defined(Q_OS_WIN)

#elif defined(Q_OS_LINUX)

#elif defined(Q_OS_MAC)

#endif

    return temp.trimmed().toUpper();
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
                    kLOG_DEBUG() << "describe_checked:" << arg.describe_checked;
                    kLOG_DEBUG() << "rxd_checked:" << arg.rxd_checked;
                    kLOG_DEBUG() << "txd_checked:" << arg.txd_checked;
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
    kLOG_DEBUG() << "item_count:" << item_count;
    for (i = 0; i < item_count; i ++) {
        item = this->ui->treeWidget->topLevelItem(i);
        arg.describe_checked = item->checkState(0);
        arg.rxd_checked = item->checkState(1);
        arg.txd_checked = item->checkState(2);
        arg.describe = item->text(0);
        arg.rxd = item->text(1);
        arg.txd = item->text(2);
        this->user_json.user_json_create_object(sub, arg);
        kLOG_DEBUG() << "sub:" << sub;
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

    if(this->isShow) {
        return;
    }

    QString buffer;
    if (this->isHexShow) {
        //buffer = MainWindow::byteArrayToHexStr(disp_buf);
    } else {
        //buffer = QUIHelper::byteArrayToAsciiStr(data);
        //buffer = QString::fromLocal8Bit(disp_buf);
    }
    //不同类型不同颜色显示
    if (dis_type == kDIS_TYPE_RX) {
        ui->textBrowser->setTextColor(QColor("dodgerblue"));
    } else if (dis_type == kDIS_TYPE_TX) {
        ui->textBrowser->setTextColor(QColor("red"));
    }

    //str = this->ui->textBrowser->toPlainText();
    time = QDateTime::currentDateTime();          //获取系统现在的时间
    str = time.toString("yyyy-MM-dd hh:mm:ss ddd") + dis_type + QString("%1").arg(disp_buf.size()) + "\r\n";         //设置显示格式
    str += MainWindow::byteArrayToHexStr(disp_buf) + "(" + QString::fromLocal8Bit(disp_buf) + ")";
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
        kLOG_DEBUG() << "available:" + com;
    }
    this->ui->comboBox_serial->clear();
    this->ui->comboBox_serial->addItems(available_ports_list);
}

void MainWindow::mainwindow_delete_QTreeWidgetItem(int currentIndex)
{
    QTreeWidgetItem *item = NULL;
    item = this->ui->treeWidget->takeTopLevelItem(currentIndex);
    kLOG_DEBUG() << "delete item:" << item;
    delete item;
    item = NULL;
}

void MainWindow::mainwindow_add_test_item()
{
    QTreeWidgetItem * item = new QTreeWidgetItem;
    item->setText(0,"测试");
    item->setCheckState(0,Qt::Checked);
    item->setText(1,"01 03 00 00 00 01 84 0A");
    item->setCheckState(1,Qt::Checked);
    item->setText(2,"01 03 02 00 01 79 84");
    item->setCheckState(2,Qt::Checked);
    this->ui->treeWidget->addTopLevelItem(item);
}

void MainWindow::mainwindow_readDataThreadTable_get(stReadDataThread *arr[256])
{
    for (unsigned long long tableId= 0; tableId < ARRAY_SIZE(this->readDataThreadTable); tableId++) {
        arr[tableId] = &this->readDataThreadTable[tableId];
    }
}

Ui::MainWindow * MainWindow::mainwindow_ui_get()
{
    return this->ui;
}

bool MainWindow::mainwindow_user_serial_isopen_get()
{
    return this->user_serial_isopen;
}

int MainWindow::mainwindow_itemCount_get()
{
    int item_count = 0;
    QTreeWidgetItemIterator it(ui->treeWidget);
    //遍历treeWidget,计算item数量
    while (*it) {
        item_count ++;
        ++it;
    }
    kLOG_DEBUG() << "item_count:" << item_count;
    return item_count;
}

void MainWindow::mainwindow_data_doWork(QByteArray &recvData)
{
    bool canSend = false;
    char * dest = NULL;
    int destDataLen = 0;
    int item_count = 0;

    QByteArray sendData;
    QTreeWidgetItem * item = NULL;

    this->mainwindow_dis_rxd_or_txd(kDIS_TYPE_RX, recvData);
    item_count = this->mainwindow_itemCount_get();
    for (int i = 0; i < item_count; i ++) {
        item = this->ui->treeWidget->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked) {
            QByteArray referData = item->text(1).toLatin1();
            kLOG_DEBUG() << "referText:" << i << referData;
#if defined(Q_OS_WIN)
            kLOG_DEBUG() << "referText:" << i << MainWindow::byteArrayToHexStr(referData.replace('\n',"\r\n"));
#elif defined(Q_OS_LINUX)
            kLOG_DEBUG() << "referText:" << i << MainWindow::byteArrayToHexStr(referText);
#elif defined(Q_OS_MAC)
            kLOG_DEBUG() << "referText:" << i << MainWindow::byteArrayToHexStr(referText);
#endif
            if(item->checkState(1) == Qt::Checked) {//接收到数据需要是十六进制数据
                destDataLen = recvData.size() * 2 + 2;
                dest = (char *)calloc(destDataLen, sizeof (char));
                if(!dest){
                    kLOG_DEBUG() << "error: calloc dest";
                    break;
                }
                memset(dest, 0, destDataLen);
                destDataLen = 0;
                destDataLen = MainWindow::HexStrToByte(referData.data(), dest, referData.size());
                kLOG_DEBUG() << "destDataLen:" << destDataLen;
                if(destDataLen > 0) {
                    if(memcmp(recvData, dest, destDataLen) == 0) {
                        if(dest) {
                            free(dest);
                            dest = NULL;
                        }
                        sendData = item->text(2).toLatin1();
                        if(item->checkState(2) == Qt::Checked) {//应答数据使用十六进制发送
                            destDataLen = sendData.size() * 2 + 2;
                            dest = (char *)calloc(destDataLen, sizeof (char));
                            if(!dest){
                                kLOG_DEBUG() << "error: calloc dest";
                                break;
                            }
                            memset(dest, 0, destDataLen);

                            destDataLen = MainWindow::HexStrToByte(sendData.data(), dest, sendData.size());
                            if(destDataLen > 0) {
                                canSend = true;
                                sendData.clear();
                                sendData = QByteArray(dest);
                            } else {
                                this->user_messagebox.user_messagebox_about(QString("item %1 您输入的应答数据有误!").arg(i));
                            }

                            if(dest) {
                                free(dest);
                                dest = NULL;
                            }
                        } else {
                            canSend = true;                            
#if defined(Q_OS_WIN)
                            sendData.replace('\n',"\r\n");
#elif defined(Q_OS_LINUX)

#elif defined(Q_OS_MAC)
                            sendData.replace('\n',"\r");
#endif
                        }
                        break;
                    }else {
                        kLOG_DEBUG() << "memcmp ....";
                    }
                } else {
                    this->user_messagebox.user_messagebox_about(QString("item %1 您输入的参考数据有误!").arg(i));
                }
            } else {//接收到数据需要是字符转
                if(referData == recvData){
                    sendData.clear();
                    sendData = item->text(2).toLatin1();
                    if(item->checkState(2) == Qt::Checked) {//应答数据使用十六进制发送
                        destDataLen = sendData.size() * 2 + 2;
                        dest = (char *)calloc(destDataLen, sizeof (char));
                        if(!dest){
                            kLOG_DEBUG() << "error: calloc dest";
                            break;
                        }
                        memset(dest, 0, destDataLen);
                        destDataLen = MainWindow::HexStrToByte(sendData.data(), dest, sendData.size());
                        if((destDataLen > 0)) {
                            canSend = true;
                            sendData.clear();
                            sendData = QByteArray(dest);
                        } else {
                            this->user_messagebox.user_messagebox_about(QString("item %1 您输入的应答数据有误!").arg(i));
                        }
                    } else {
                        canSend = true;
#if defined(Q_OS_WIN)
                        sendData.replace('\n',"\r\n");
#elif defined(Q_OS_LINUX)

#elif defined(Q_OS_MAC)
                        sendData.replace('\n',"\r");
#endif
                    }
                    break;
                }
            }
        }
    }

    if(canSend){
        this->mainwindow_dis_rxd_or_txd(kDIS_TYPE_TX, sendData);
        kLOG_DEBUG() << "sendData.length:" << sendData.length() << "sendData.size:" << sendData.size() << "constData:" << sendData.constData();
        this->user_serial.user_serial_wirte(sendData.constData(), sendData.size());
    }
}



void Worker::mainwindow_readDataDoworkSlot(MainWindow * window)
{
    kLOG_DEBUG() << "mainwindow_readDataDoworkSlot";
    kLOG_DEBUG() << "receive the execute signal---------------------------------";
    kLOG_DEBUG() << "current thread ID:"<<QThread::currentThreadId();

    unsigned long long tableId = 0;
    QByteArray recv_buf;
    stReadDataThread * pReadDataThreadTable[256] = {0};


    recv_buf = window->user_serial.user_serial_read();
    kLOG_DEBUG() << "recv_buf:" << recv_buf << "size:" << recv_buf.size();
    window->mainwindow_data_doWork(recv_buf);

    window->mainwindow_readDataThreadTable_get(pReadDataThreadTable);
    for (tableId = 0; tableId < ARRAY_SIZE(pReadDataThreadTable); tableId++) {
        if(pReadDataThreadTable[tableId]->id == 0) break;
    }
    kLOG_DEBUG() << "finish the work and sent the resultReady signal\n";
    pReadDataThreadTable[tableId]->id = (unsigned long long)QThread::currentThreadId();
    emit readDataResultReadySignal((unsigned long long)(QThread::currentThreadId()));
}

void Worker::mainwindow_readDataDeleteLater(Worker * self)
{
    if(self) {
        kLOG_DEBUG() << "mainwindow_readDataDeleteLater";
        delete self;
    }
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
//    kLOG_DEBUG() << "baud_rate:";
//    kLOG_DEBUG() << serial_arg.baud_rate;
//    kLOG_DEBUG() << "data_bit:";
//    kLOG_DEBUG() << serial_arg.data_bit;
//    kLOG_DEBUG() << "stop_bit:";
//    kLOG_DEBUG() << serial_arg.stop_bit;
//    kLOG_DEBUG() << "parity:";
//    kLOG_DEBUG() << serial_arg.parity;
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
    kLOG_DEBUG() << "item_count:" << item_count;
    for (i = 0; i < item_count; i ++) {
        QTreeWidgetItem * item = this->ui->treeWidget->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked) {
            output_str = item->text(2);
            kLOG_DEBUG() << output_str.toLatin1().data();
            if(item->checkState(2) == Qt::Checked) {//十六进制字符串，转成十六进制再发送
                int data_len = 0;
                QByteArray byteArry = output_str.toLatin1();
                char *dest = (char *)calloc(byteArry.size() * 2 + 2, sizeof (char));
                if(dest ==NULL) {
                    kLOG_DEBUG() << "error: calloc dest";
                    break;
                }
                kLOG_DEBUG() << "byteArry:" << byteArry << "size:" << byteArry.size();
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
    kLOG_DEBUG() << "*********readData**********";
    QByteArray recvData;
    recvData = this->user_serial.user_serial_read();
    kLOG_DEBUG() << "recvData:" << recvData << "size:" << recvData.size();
    this->mainwindow_data_doWork(recvData);
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
//            kLOG_DEBUG() << "delete item:" << item;
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
    Ui::User_dialog *user_dialog_ui = NULL;

    column = 0;
    index = this->ui->treeWidget->indexOfTopLevelItem(item);
    kLOG_DEBUG() << "******column:" << column;
    kLOG_DEBUG() << "******index:" << index;
    textEdit_d_str = item->text(0);
    textEdit_r_str = item->text(1);
    textEdit_t_str = item->text(2);
    kLOG_DEBUG() << "******textEdit_d_str:" + textEdit_d_str;
    kLOG_DEBUG() << "******textEdit_r_str:" + textEdit_r_str;
    kLOG_DEBUG() << "******textEdit_t_str:" + textEdit_t_str;

    user_dialog_ui = this->user_dialog.user_dialog_get_ui();
    user_dialog_ui->textEdit_d->setText(textEdit_d_str);
    user_dialog_ui->textEdit_r->setText(textEdit_r_str);
    user_dialog_ui->textEdit_t->setText(textEdit_t_str);

    this->user_dialog.show();
    ret = this->user_dialog.exec();
    if(ret == QDialog::Accepted){//点击确定按钮走这里
        kLOG_DEBUG()<<"accept";
        textEdit_d_str = user_dialog_ui->textEdit_d->toPlainText();
        textEdit_r_str = user_dialog_ui->textEdit_r->toPlainText();
        textEdit_t_str = user_dialog_ui->textEdit_t->toPlainText();
        item->setText(0, textEdit_d_str);
        item->setText(1, textEdit_r_str);
        item->setText(2, textEdit_t_str);
        this->ui->treeWidget->addTopLevelItem(item);
    }else if(ret == QDialog::Rejected){//点击取消按钮走这里
        kLOG_DEBUG()<<"reject";
    }
}

void MainWindow::mainwindow_qcombobox_activated_slot(int index)
{
    int ret = -1;
    QString lineEdit_str;
    Ui::User_baud_rate_dialog *user_baud_rate_dialog_ui = NULL;
    //kLOG_DEBUG() <<  "mainwindow_qcombobox_index_xchanged_slot index = " << index;
    if(index == 1) {
        user_baud_rate_dialog_ui = this->user_baud_dialog.user_baud_rate_dialog_get_ui();
        user_baud_rate_dialog_ui->lineEdit->clear();

        this->user_baud_dialog.setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        this->user_baud_dialog.setWindowFlags(this->windowFlags());
        this->user_baud_dialog.show();
        ret = this->user_baud_dialog.exec();
        if(ret == QDialog::Accepted){//点击确定按钮走这里
            kLOG_DEBUG()<<"accept";
            lineEdit_str = user_baud_rate_dialog_ui->lineEdit->text();
            this->ui->comboBox_baud_rate->setItemText(0, lineEdit_str);
        }else if(ret == QDialog::Rejected){//点击取消按钮走这里
            kLOG_DEBUG()<<"reject";
        }
        this->ui->comboBox_baud_rate->setCurrentIndex(0);
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
        kLOG_DEBUG()<<"accept";
    }else if(ret == QDialog::Rejected){//点击取消按钮走这里
        kLOG_DEBUG()<<"reject";
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
        kLOG_DEBUG()<<"accept";
    }else if(ret == QDialog::Rejected){//点击取消按钮走这里
        kLOG_DEBUG()<<"reject";
    }
}

void MainWindow::on_action_save_triggered()
{
    QString save_path;
    QString default_path;

    save_path = QFileDialog::getSaveFileName(this, tr("保存配置文件"), tr(kCFG_JSON_ROOT_PATH), tr("配置文件(*.json);;所有文件(*);"));
    if(!save_path.isNull()){
        kLOG_DEBUG() << "save_path=" << save_path;
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
//        kLOG_DEBUG() << "str_path:" << str_path;
//    }
    QString str_path;

    str_path = QFileDialog::getOpenFileName(this, tr("选择配置文件"), tr(kCFG_JSON_ROOT_PATH), tr("配置文件(*.json);;所有文件(*);"));
    if(!str_path.isNull()){
        this->ui->treeWidget->clear();
        kLOG_DEBUG() << "path=" << str_path;
        this->mainwindow_load_cfg(str_path);
    }
}

void MainWindow::on_action_new_triggered()
{
    QString save_path;
    QString default_path;

    save_path = QFileDialog::getSaveFileName(this, tr("保存配置文件"), tr(kCFG_JSON_ROOT_PATH), tr("配置文件(*.json);;所有文件(*);"));
    if(!save_path.isNull()){
        kLOG_DEBUG() << "save_path=" << save_path;
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

void MainWindow::handleResults(const unsigned long long threadId)
{
    //kLOG_DEBUG()<<"receive the resultReady signal---------------------------------";
    //kLOG_DEBUG()<<"current thread ID:"<<QThread::currentThreadId()<<'\n';
    //kLOG_DEBUG()<<"the last result is:"<<threadId;
    unsigned long long tableId = 0;
    for (tableId = 0; tableId < ARRAY_SIZE(readDataThreadTable); tableId++) {
        //kLOG_DEBUG()<<"readDataThreadTable["<<tableId<<"].id:"<<readDataThreadTable[tableId].id;
        if(readDataThreadTable[tableId].id == threadId){
            kLOG_DEBUG() << "delete readDataThread:" << threadId;
            readDataThreadTable[tableId].id = 0;
            readDataThreadTable[tableId].readDataThread->quit();
            readDataThreadTable[tableId].readDataThread->wait();
            delete readDataThreadTable[tableId].readDataThread;
            readDataThreadTable[tableId].readDataThread = nullptr;
            break;
        }
    }
}


void MainWindow::on_chStopDis_clicked(bool checked)
{
    kLOG_DEBUG() << "on chStopDis clicked:" << checked;
    this->isShow = checked;
}
