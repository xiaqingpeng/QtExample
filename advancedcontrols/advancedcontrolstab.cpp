#include "advancedcontrolstab.h"

AdvancedControlsTab::AdvancedControlsTab(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    
    createProgressGroup();
    createDateTimeGroup();
    createSplitterGroup();
}

void AdvancedControlsTab::createProgressGroup()
{
    QGroupBox *progressGroup = new QGroupBox("进度指示器");
    QGridLayout *progressLayout = new QGridLayout(progressGroup);
    
    QProgressBar *progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(75);
    QProgressBar *chunkProgress = new QProgressBar();
    chunkProgress->setRange(0, 100);
    chunkProgress->setValue(40);
    chunkProgress->setTextVisible(false);
    chunkProgress->setStyleSheet("QProgressBar::chunk { background-color: #e74c3c; }");
    QLCDNumber *lcdNumber = new QLCDNumber();
    lcdNumber->display(88);
    QDial *dial = new QDial();
    dial->setRange(0, 100);
    dial->setValue(60);
    
    progressLayout->addWidget(new QLabel("进度条:"), 0, 0);
    progressLayout->addWidget(progressBar, 0, 1);
    progressLayout->addWidget(new QLabel("分块进度:"), 1, 0);
    progressLayout->addWidget(chunkProgress, 1, 1);
    progressLayout->addWidget(new QLabel("LCD显示:"), 2, 0);
    progressLayout->addWidget(lcdNumber, 2, 1);
    progressLayout->addWidget(new QLabel("拨号盘:"), 3, 0);
    progressLayout->addWidget(dial, 3, 1);
    mainLayout->addWidget(progressGroup);
}

void AdvancedControlsTab::createDateTimeGroup()
{
    QGroupBox *datetimeGroup = new QGroupBox("日期时间");
    QFormLayout *datetimeLayout = new QFormLayout(datetimeGroup);
    
    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate());
    QTimeEdit *timeEdit = new QTimeEdit(QTime::currentTime());
    QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    QCalendarWidget *calendar = new QCalendarWidget();
    
    datetimeLayout->addRow("日期选择:", dateEdit);
    datetimeLayout->addRow("时间选择:", timeEdit);
    datetimeLayout->addRow("日期时间:", dateTimeEdit);
    datetimeLayout->addRow("日历控件:", calendar);
    mainLayout->addWidget(datetimeGroup);
}

void AdvancedControlsTab::createSplitterGroup()
{
    QGroupBox *splitterGroup = new QGroupBox("分割视图");
    QVBoxLayout *splitterLayout = new QVBoxLayout(splitterGroup);
    
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    QTextEdit *leftEdit = new QTextEdit("左侧区域 - 可以放置任何内容\n\n尝试拖动分割线调整大小");
    QTextEdit *rightEdit = new QTextEdit("右侧区域 - 分割视图非常实用\n\n适用于需要同时查看两个相关面板的场景");
    splitter->addWidget(leftEdit);
    splitter->addWidget(rightEdit);
    splitter->setSizes({200, 400});
    splitterLayout->addWidget(splitter);
    mainLayout->addWidget(splitterGroup);
}