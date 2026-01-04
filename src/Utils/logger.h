#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

// 日志控制宏
#ifdef QT_NO_DEBUG_OUTPUT
    #define LOG_DEBUG(msg) do {} while(0)
    #define LOG_INFO(msg) do {} while(0)
    #define LOG_WARNING(msg) qWarning() << msg
    #define LOG_ERROR(msg) qCritical() << msg
#else
    #define LOG_DEBUG(msg) qDebug() << msg
    #define LOG_INFO(msg) qInfo() << msg
    #define LOG_WARNING(msg) qWarning() << msg
    #define LOG_ERROR(msg) qCritical() << msg
#endif

// 性能日志（仅在调试模式下输出）
#ifdef QT_NO_DEBUG_OUTPUT
    #define LOG_PERF(msg) do {} while(0)
#else
    #define LOG_PERF(msg) qDebug() << "[PERF]" << msg
#endif

// 用户操作日志（总是输出，用于分析）
#define LOG_USER_ACTION(msg) qInfo() << "[USER]" << msg

#endif // LOGGER_H