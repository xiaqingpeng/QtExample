#ifndef COMMON_H
#define COMMON_H

#include <QDebug>

// 自定义日志宏：打印文件名、行号和日志信息
#define LOG_DEBUG() qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"
#define LOG_INFO(msg) qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]" << (msg)

#endif // COMMON_H