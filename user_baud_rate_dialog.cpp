#include "user_baud_rate_dialog.h"


User_baud_rate_dialog::User_baud_rate_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_user_baud_rate_dialog::User_baud_rate_dialog_ui)
{
    ui->setupUi(this);
    this->setWindowTitle("自定义波特率");
    this->setFixedWidth(320);
    this->setFixedHeight(160);
    this->setModal(false);

    QRegExp regx("[^0][0-9]{7}$");//只能输入数字，第一个数字不能为0，一共能输入8个数字
    QValidator *validator = new QRegExpValidator(regx, this->ui->lineEdit);
    this->ui->lineEdit->setValidator(validator);

    connect(this->ui->pushButton_ok, &QPushButton::clicked, this, &User_baud_rate_dialog::on_pushButton_ok_clicked);
    connect(this->ui->pushButton_cancel, &QPushButton::clicked, this, &User_baud_rate_dialog::on_pushButton_cancel_clicked);
}

User_baud_rate_dialog::~User_baud_rate_dialog()
{
    delete ui;
}

void User_baud_rate_dialog::on_pushButton_ok_clicked()
{
    accept();
}

void User_baud_rate_dialog::on_pushButton_cancel_clicked()
{
    reject();
}

Ui_user_baud_rate_dialog::User_baud_rate_dialog_ui *User_baud_rate_dialog::user_baud_rate_dialog_get_ui()
{
    return this->ui;
}
