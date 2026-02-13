#include "gaugewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <QDebug>

GaugeWidget::GaugeWidget(QWidget *parent)
    : QWidget(parent)
    , m_value(0.0)
    , m_title("")
    , m_unit("%")
{
    setMinimumSize(200, 200);
}

void GaugeWidget::setValue(double value)
{
    m_value = qBound(0.0, value, 100.0);
    update();
}

void GaugeWidget::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void GaugeWidget::setUnit(const QString &unit)
{
    m_unit = unit;
    update();
}

void GaugeWidget::setDetails(const QStringList &details)
{
    m_details = details;
    update();
}

void GaugeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect rect = this->rect();
    drawGauge(painter, rect);
}

void GaugeWidget::drawGauge(QPainter &painter, const QRect &rect)
{
    // 计算仪表盘的中心和半径
    int size = qMin(rect.width(), rect.height());
    int padding = 20;
    int gaugeSize = size - padding * 2;
    QPoint center = rect.center();
    
    QRect gaugeRect(
        center.x() - gaugeSize / 2,
        center.y() - gaugeSize / 2,
        gaugeSize,
        gaugeSize
    );
    
    // 绘制背景弧（灰色）
    drawArc(painter, gaugeRect, 180, 180, QColor(230, 230, 230));
    
    // 根据值绘制不同颜色的弧
    double angle = valueToAngle(m_value);
    QColor color;
    if (m_value < 50) {
        color = QColor(95, 189, 104); // 绿色
    } else if (m_value < 80) {
        color = QColor(255, 193, 7);   // 黄色
    } else {
        color = QColor(220, 53, 69);   // 红色
    }
    
    drawArc(painter, gaugeRect, 180, angle, color);
    
    // 绘制指针
    drawNeedle(painter, gaugeRect, angle);
    
    // 绘制文本
    drawText(painter, gaugeRect);
    
    // 绘制详细信息
    if (!m_details.isEmpty()) {
        QRect detailsRect(
            rect.left(),
            gaugeRect.bottom() + 10,
            rect.width(),
            rect.bottom() - gaugeRect.bottom() - 10
        );
        drawDetails(painter, detailsRect);
    }
}

void GaugeWidget::drawArc(QPainter &painter, const QRect &rect, double startAngle, double spanAngle, const QColor &color)
{
    QPen pen(color, 15);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    
    QRect arcRect = rect.adjusted(15, 15, -15, -15);
    
    QPainterPath path;
    path.arcMoveTo(arcRect, startAngle);
    path.arcTo(arcRect, startAngle, spanAngle);
    
    painter.drawPath(path);
}

void GaugeWidget::drawNeedle(QPainter &painter, const QRect &rect, double angle)
{
    QPoint center = rect.center();
    double radius = rect.width() / 2.0 - 20;
    
    // 计算指针终点
    double radian = qDegreesToRadians(180 + angle);
    double endX = center.x() + radius * qCos(radian);
    double endY = center.y() + radius * qSin(radian);
    
    QPen pen(QColor(100, 100, 100), 3);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    
    painter.drawLine(center, QPointF(endX, endY));
    
    // 绘制中心圆点
    painter.setBrush(QBrush(QColor(100, 100, 100)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, 8, 8);
}

void GaugeWidget::drawText(QPainter &painter, const QRect &rect)
{
    QPoint center = rect.center();
    
    // 绘制标题
    if (!m_title.isEmpty()) {
        QFont titleFont("Arial", 14, QFont::Bold);
        painter.setFont(titleFont);
        painter.setPen(QColor(50, 50, 50));
        
        QRect titleRect(rect.left(), rect.top() + 10, rect.width(), 30);
        painter.drawText(titleRect, Qt::AlignCenter, m_title);
    }
    
    // 绘制数值
    QFont valueFont("Arial", 24, QFont::Bold);
    painter.setFont(valueFont);
    painter.setPen(QColor(30, 30, 30));
    
    QString valueText = QString::number(m_value, 'f', 1) + m_unit;
    QRect valueRect(rect.left(), center.y() - 15, rect.width(), 40);
    painter.drawText(valueRect, Qt::AlignCenter, valueText);
}

void GaugeWidget::drawDetails(QPainter &painter, const QRect &rect)
{
    QFont detailFont("Arial", 10);
    painter.setFont(detailFont);
    painter.setPen(QColor(100, 100, 100));
    
    int yOffset = 0;
    int lineHeight = 18;
    
    for (const QString &detail : m_details) {
        QRect detailRect(rect.left(), rect.top() + yOffset, rect.width(), lineHeight);
        painter.drawText(detailRect, Qt::AlignCenter, detail);
        yOffset += lineHeight;
    }
}

double GaugeWidget::valueToAngle(double value) const
{
    // 将 0-100 的值转换为 0-180 的角度（从左侧开始）
    return (value / 100.0) * 180.0;
}

