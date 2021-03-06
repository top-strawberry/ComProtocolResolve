#include "user_messagebox.h"

User_messagebox::User_messagebox()
{
    //this->setAttribute( Qt::WA_DeleteOnClose ); //makes sure the msgbox is deleted automatically when closed
}

User_messagebox::~User_messagebox()
{

}

int User_messagebox::user_messagebox_about(QString msg_str)
{
    static QMessageBox * msg = NULL;

    if(msg) {
        msg->close();
        delete msg;
        msg = NULL;
    }
    msg = new QMessageBox;
    msg->setModal(false); // if you want it non-modal
    msg->setWindowTitle("提示");
    msg->setText(msg_str);
    msg->show();
    msg->exec();
    delete msg;
    msg = NULL;
    return 0;
}
