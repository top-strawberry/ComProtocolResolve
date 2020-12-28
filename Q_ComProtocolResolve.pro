QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    user_about_dialog.cpp \
    user_baud_rate_dialog.cpp \
    user_contact_dialog.cpp \
    user_dialog.cpp \
    user_json.cpp \
    user_messagebox.cpp \
    user_serial.cpp

HEADERS += \
    mainwindow.h \
    user_about_dialog.h \
    user_baud_rate_dialog.h \
    user_contact_dialog.h \
    user_dialog.h \
    user_json.h \
    user_messagebox.h \
    user_serial.h

FORMS += \
    mainwindow.ui \
    user_about_dialog.ui \
    user_baud_rate_dialog.ui \
    user_contact_dialog.ui \
    user_dialog.ui

TRANSLATIONS += \
    Q_ComProtocolResolve_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pic/pic.qrc
