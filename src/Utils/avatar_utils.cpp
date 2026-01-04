#include "avatar_utils.h"
#include <QBitmap>
#include <QPainterPath>

QPixmap AvatarUtils::createCircularAvatar(const QPixmap &source, int size)
{
    if (source.isNull()) {
        return createDefaultAvatar(size);
    }
    
    // 创建指定大小的圆形图片
    QPixmap result(size, size);
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 设置圆形裁剪区域
    QPainterPath clipPath;
    clipPath.addEllipse(0, 0, size, size);
    painter.setClipPath(clipPath);
    
    // 计算缩放，确保图片能够完全覆盖圆形区域
    qreal scaleX = static_cast<qreal>(size) / source.width();
    qreal scaleY = static_cast<qreal>(size) / source.height();
    qreal scale = qMax(scaleX, scaleY); // 使用较大的缩放比例确保完全覆盖
    
    int scaledWidth = static_cast<int>(source.width() * scale);
    int scaledHeight = static_cast<int>(source.height() * scale);
    
    // 缩放图片
    QPixmap scaledPixmap = source.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 计算居中位置
    int x = (size - scaledWidth) / 2;
    int y = (size - scaledHeight) / 2;
    
    // 绘制图片，确保完全填充圆形
    painter.drawPixmap(x, y, scaledPixmap);
    painter.end();
    
    return result;
}

QPixmap AvatarUtils::createDefaultAvatar(int size)
{
    QPixmap defaultAvatar(size, size);
    defaultAvatar.fill(Qt::transparent);
    QPainter painter(&defaultAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 创建现代简约的渐变背景
    QRadialGradient gradient(size/2, size/2, size/2);
    gradient.setColorAt(0, QColor("#f1f5f9"));
    gradient.setColorAt(1, QColor("#e2e8f0"));
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);
    
    // 绘制现代用户图标
    painter.setBrush(QBrush(QColor("#94a3b8")));
    
    // 根据尺寸调整图标大小
    int headSize = size * 0.4;
    int bodyWidth = size * 0.7;
    int bodyHeight = size * 0.4;
    int headX = (size - headSize) / 2;
    int headY = size * 0.25;
    int bodyX = (size - bodyWidth) / 2;
    int bodyY = size * 0.7;
    
    painter.drawEllipse(headX, headY, headSize, headSize); // 头部
    painter.drawEllipse(bodyX, bodyY, bodyWidth, bodyHeight); // 身体
    
    return defaultAvatar;
}

QPixmap AvatarUtils::createCircularAvatarWithBorder(const QPixmap &source, int size, 
                                                    int borderWidth, const QColor &borderColor)
{
    // 创建带边框的圆形头像
    QPixmap result(size, size);
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制边框
    painter.setBrush(QBrush(borderColor));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);
    
    // 绘制内部头像
    int innerSize = size - 2 * borderWidth;
    QPixmap innerAvatar = createCircularAvatar(source, innerSize);
    painter.drawPixmap(borderWidth, borderWidth, innerAvatar);
    
    return result;
}