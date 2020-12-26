#ifndef USER_CONTACT_DIALOG_H
#define USER_CONTACT_DIALOG_H

#include <QDialog>

namespace Ui {
class User_contact_dialog;
}

class User_contact_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit User_contact_dialog(QWidget *parent = nullptr);
    ~User_contact_dialog();

private:
    Ui::User_contact_dialog *ui;
};

#endif // USER_CONTACT_DIALOG_H
