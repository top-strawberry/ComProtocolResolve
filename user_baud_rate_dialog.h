#ifndef USER_BAUD_RATE_DIALOG_H
#define USER_BAUD_RATE_DIALOG_H

#include "ui_user_baud_rate_dialog.h"
#include <QDialog>
#include <QRegExp>
#include <QValidator>

namespace Ui_user_baud_rate_dialog {
class User_baud_rate_dialog_ui;
}

class User_baud_rate_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit User_baud_rate_dialog(QWidget *parent = nullptr);
    ~User_baud_rate_dialog();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui_user_baud_rate_dialog::User_baud_rate_dialog_ui *ui;
public:
    Ui_user_baud_rate_dialog::User_baud_rate_dialog_ui * user_baud_rate_dialog_get_ui(void);
};

#endif // USER_BAUD_RATE_DIALOG_H
