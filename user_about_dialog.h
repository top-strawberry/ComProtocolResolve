#ifndef USER_ABOUT_DIALOG_H
#define USER_ABOUT_DIALOG_H

#include <QDialog>

namespace Ui {
    class User_about_dialog;
}

class User_about_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit User_about_dialog(QWidget *parent = nullptr);
    ~User_about_dialog();

private:
    Ui::User_about_dialog *ui;
};

#endif // USER_ABOUT_DIALOG_H
