#ifndef GAUGEWIDGET_H
#define GAUGEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QString>

/**
 * @brief 自定义仪表盘（Gauge）Widget
 * 
 * 用于显示百分比数据，模拟 ECharts 的 gauge 图表
 */
class GaugeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)

public:
    explicit GaugeWidget(QWidget *parent = nullptr);
    
    // 属性访问器
    double value() const { return m_value; }
    void setValue(double value);
    
    QString title() const { return m_title; }
    void setTitle(const QString &title);
    
    QString unit() const { return m_unit; }
    void setUnit(const QString &unit);
    
    // 设置详细信息（用于显示在底部）
    void setDetails(const QStringList &details);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_value;           // 当前值（0-100）
    QString m_title;          // 标题
    QString m_unit;           // 单位（如 "%", "GB"）
    QStringList m_details;    // 详细信息列表
    
    // 绘制方法
    void drawGauge(QPainter &painter, const QRect &rect);
    void drawArc(QPainter &painter, const QRect &rect, double startAngle, double spanAngle, const QColor &color);
    void drawNeedle(QPainter &painter, const QRect &rect, double angle);
    void drawText(QPainter &painter, const QRect &rect);
    void drawDetails(QPainter &painter, const QRect &rect);
    
    // 计算角度
    double valueToAngle(double value) const;
};

#endif // GAUGEWIDGET_H

