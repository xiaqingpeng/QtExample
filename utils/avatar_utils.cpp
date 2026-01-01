#include "avatar_utils.h"
#include <QBitmap>
#include <QPainterPath>

QPixmap AvatarUtils::createCircularAvatar(const QPixmap &source, int size)
{
    if (source.isNull()) {
        return createDefaultAvatar(size);
    }
    
    // 创建指定大小的圆形图片
    QPixmap circularPixmap(size, size);
    circularPixmap.fill(Qt::transparent);
    
    // 计算缩放比例，确保图片完全填充圆形区域
    qreal scale = qMax(static_cast<qreal>(size) / source.width(), 
                       static_cast<qreal>(size) / source.height());
    
    // 缩放图片
    QPixmap scaledPixmap = source.scaled(
        source.width() * scale, 
        source.height() * scale, 
        Qt::KeepAspectRatio, 
        Qt::SmoothTransformation
    );
    
    // 计算居中位置
    int x = (scaledPixmap.width() - size) / 2;
    int y = (scaledPixmap.height() - size) / 2;
    
    // 裁剪到指定大小
    QPixmap croppedPixmap = scaledPixmap.copy(x, y, size, size);
    
    // 创建最终的圆形图片
    QPainter finalPainter(&circularPixmap);
    finalPainter.setRenderHint(QPainter::Antialiasing);
    finalPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    
    // 设置圆形裁剪路径
    QPainterPath clipPath;
    clipPath.addEllipse(0, 0, size, size);
    finalPainter.setClipPath(clipPath);
    
    // 绘制图片
    finalPainter.drawPixmap(0, 0, croppedPixmap);
    finalPainter.end();
    
    return circularPixmap;
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