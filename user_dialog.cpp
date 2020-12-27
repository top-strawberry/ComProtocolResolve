#include "user_dialog.h"


User_dialog::User_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_user_dialog::User_dialog)
{
    this->ui->setupUi(this);
    this->setWindowTitle("ComProtocolResolve dialog(by stawberry)");
    this->setFixedWidth(800);
    this->setFixedHeight(480);
    this->setModal(false);
    connect(this->ui->pushButton_ok, &QPushButton::clicked, this, &User_dialog::user_dialog_slotOk);
    connect(this->ui->pushButton_cancel, &QPushButton::clicked, this, &User_dialog::user_dialog_slotCancel);

}

User_dialog::~User_dialog()
{
    delete ui;
}

Ui_user_dialog::User_dialog *User_dialog::user_dialog_get_ui()
{
    return this->ui;
}


void User_dialog::user_dialog_slotOk()
{
    accept();
}

void User_dialog::user_dialog_slotCancel()
{
    reject();    //隐藏对话框，并且dlg.exec()返回QDialog::Rejected
    //hide();    //隐藏对话框，并且dlg.exec()返回QDialog::Rejected
    //close();   //隐藏对话框，并且dlg.exec()返回QDialog::Rejected
}
