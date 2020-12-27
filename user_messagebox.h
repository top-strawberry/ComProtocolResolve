#ifndef USER_MESSAGEBOX_H
#define USER_MESSAGEBOX_H

#include <QMessageBox>
#include <QString>

class User_messagebox :public QMessageBox
{
public:
    User_messagebox();
    ~User_messagebox();

public:
    int user_messagebox_about(QString msg_str);
};

#endif // USER_MESSAGEBOX_H
