#include "user_json.h"

User_json::User_json()
{

}

User_json::~User_json()
{

}

int User_json::user_json_create_object(QJsonObject &obj, User_json_create_object_arg arg)
{
    qDebug() << "describe_checked:" << arg.describe_checked;
    qDebug() << "rxd_checked:" << arg.rxd_checked;
    qDebug() << "txd_checked:" << arg.txd_checked;
    qDebug() << "describe:" << arg.describe;
    qDebug() << "rxd:" << arg.rxd;
    qDebug() << "txd:" << arg.txd;
    obj.insert("describe_checked", arg.describe_checked);
    obj.insert("rxd_checked", arg.rxd_checked);
    obj.insert("txd_checked", arg.txd_checked);
    obj.insert("describe", arg.describe);
    obj.insert("rxd", arg.rxd);
    obj.insert("txd", arg.txd);

    return 0;
}

int User_json::user_json_parse_object(QJsonObject &obj, User_json_create_object_arg &arg)
{
    QJsonValue value;

    if(obj.contains("describe_checked")) {
        value = obj.value("describe_checked");
        if(value.isBool()) {
            arg.describe_checked = value.toBool();
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    if(obj.contains("rxd_checked")) {
        value = obj.value("rxd_checked");
        if(value.isBool()) {
            arg.rxd_checked = value.toBool();
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    if(obj.contains("txd_checked")) {
        value = obj.value("txd_checked");
        if(value.isBool()) {
            arg.txd_checked = value.toBool();
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    if(obj.contains("describe")) {
        value = obj.value("describe");
        if(value.isString()) {
            arg.describe = value.toString();
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    if(obj.contains("rxd")) {
        value = obj.value("rxd");
        if(value.isString()){
            arg.rxd = value.toString();
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    if(obj.contains("txd")) {
        value = obj.value("txd");
        if(value.isString()) {
            arg.txd = value.toString();
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}
