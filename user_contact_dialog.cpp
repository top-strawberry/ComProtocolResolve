#include "user_contact_dialog.h"
#include "ui_user_contact_dialog.h"

User_contact_dialog::User_contact_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::User_contact_dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("作者");
    this->setFixedWidth(370);
    this->setFixedHeight(230);
}

User_contact_dialog::~User_contact_dialog()
{
    delete ui;
}
