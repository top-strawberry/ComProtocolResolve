#ifndef __COMMON_H
#define __COMMON_H

#include <QDebug>
/* Exported macro ------------------------------------------------------------*/
#define PRINTF 0

#if PRINTF
    #define INFO_PRINTF 1
    #define DEBUG_PRINTF 1
    #define ERROR_PRINTF 1
    #define WARING_PRINTF 1
#endif


//#define qDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug
//#define qInfo QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).info
//#define qWarning QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).warning
//#define qCritical QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).critical
//#define qFatal QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).fatal

#if INFO_PRINTF
    #define kLOG_INFO QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).info
#else
    #define kLOG_INFO QMessageLogger().noDebug
#endif

#if DEBUG_PRINTF
    #define kLOG_DEBUG QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug
#else
#define kLOG_DEBUG QMessageLogger().noDebug
#endif

#if ERROR_PRINTF
    #define kLOG_ERROR QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).fatal
#else
    #define kLOG_ERROR QMessageLogger().noDebug
#endif

#if WARING_PRINTF
    #define kLOG_WARNING QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).warning
#else
    #define kLOG_WARNING QMessageLogger().noDebug
#endif

#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

#endif // COMMON_H
