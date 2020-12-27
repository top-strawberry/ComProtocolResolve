#ifndef USER_JSON_H
#define USER_JSON_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

#include <QString>
#include <QDebug>


typedef struct User_json_create_object_arg
{
    bool describe_checked;
    bool rxd_checked;
    bool txd_checked;
    QString describe;
    QString rxd;
    QString txd;
} User_json_create_object_arg;

class User_json :public QJsonDocument, public QJsonObject, public QJsonArray, public QJsonValue, public QJsonParseError
{
public:
    explicit User_json();
    ~User_json();

private:
    User_json_create_object_arg user_json_create_object_arg;

public:
    int user_json_create_object(QJsonObject &obj, User_json_create_object_arg arg);
    int user_json_parse_object(QJsonObject &obj, User_json_create_object_arg &arg);

};

#endif // USER_JSON_H
