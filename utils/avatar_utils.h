#ifndef AVATAR_UTILS_H
#define AVATAR_UTILS_H

#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QRadialGradient>
#include <QPainterPath>

class AvatarUtils
{
public:
    // 创建圆形头像
    static QPixmap createCircularAvatar(const QPixmap &source, int size);
    
    // 创建默认头像
    static QPixmap createDefaultAvatar(int size);
    
    // 创建带边框的圆形头像
    static QPixmap createCircularAvatarWithBorder(const QPixmap &source, int size, 
                                                  int borderWidth = 3, 
                                                  const QColor &borderColor = Qt::white);

private:
    AvatarUtils() = default; // 工具类，不允许实例化
};

#endif // AVATAR_UTILS_H