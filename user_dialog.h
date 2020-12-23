#ifndef USER_DIALOG_H
#define USER_DIALOG_H

#include "ui_user_dialog.h"
#include <QDialog>
#include <QDebug>

namespace Ui_user_dialog {
class User_dialog;
}

class User_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit User_dialog(QWidget *parent = nullptr);
    ~User_dialog();

private:
    Ui_user_dialog::User_dialog *ui;
public:
    Ui_user_dialog::User_dialog * user_dialog_get_ui(void);

private slots:
    void user_dialog_slotOk();
    void user_dialog_slotCancel();
};

#endif // USER_DIALOG_H
