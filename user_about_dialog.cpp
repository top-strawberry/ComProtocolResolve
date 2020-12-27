#include "user_about_dialog.h"
#include "ui_user_about_dialog.h"

User_about_dialog::User_about_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::User_about_dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("关于");
    this->setFixedWidth(320);
    this->setFixedHeight(240);
    this->setModal(false);
}

User_about_dialog::~User_about_dialog()
{
    delete ui;
}
