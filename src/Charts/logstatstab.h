#ifndef LOGSTATSTAB_H
#define LOGSTATSTAB_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "networkmanager.h"
#include "theme_manager.h"

class LogStatsTab : public QWidget
{
    Q_OBJECT

public:
    explicit LogStatsTab(QWidget *parent = nullptr);
    ~LogStatsTab();

private slots:
    // 当筛选条件改变时重新获取数据
    void onFilterChanged();
    // 当时间筛选条件改变时重新获取数据
    void onTimeFilterChanged();
    // 快捷时间按钮点击事件
    void onTimeShortcutClicked(int days);
    // 获取日志数据
    void fetchLogData();
    // 上一页
    void onPreviousPage();
    // 下一页
    void onNextPage();
    // 跳转到指定页
    void onGoToPage();
    // 主题变化事件
    void applyTheme();

private:
    // 初始化UI
    void setupUI();
    // 创建筛选控件
    void setupFilterControls();
    // 创建表格
    void setupTable();
    // 创建分页控件
    void setupPaginationControls();
    // 更新按钮高亮状态
    void updateButtonHighlight(int days);
    // 更新分页信息
    void updatePaginationInfo();

    // 网络管理器
    NetworkManager *m_networkManager;

    // 布局
    QHBoxLayout *m_filterLayout;      // 筛选控件布局
    QHBoxLayout *m_paginationLayout; // 分页控件布局

    // 筛选控件
    QComboBox *m_methodCombo;   // 方法选择下拉框
    QComboBox *m_platformCombo; // 平台选择下拉框
    QDateTimeEdit *m_startTimeEdit; // 开始时间选择器
    QDateTimeEdit *m_endTimeEdit;   // 结束时间选择器

    // 快捷时间按钮
    QPushButton *m_btnToday;
    QPushButton *m_btnYesterday;
    QPushButton *m_btnLast7Days;
    QPushButton *m_btnLast30Days;
    QPushButton *m_btnClearTime;
    
    // 当前高亮的天数（用于主题切换时保持状态）
    int m_currentHighlightedDays;

    // 表格
    QTableWidget *m_tableWidget;

    // 分页控件
    QLabel *m_pageInfoLabel;
    QPushButton *m_btnPrevious;
    QPushButton *m_btnNext;
    QPushButton *m_btnGoToPage;
    QComboBox *m_pageCombo;  // 页码选择器

    // 分页参数
    int m_currentPage;
    int m_pageSize;
    int m_totalPages;
    int m_totalRecords;
};

#endif // LOGSTATSTAB_H