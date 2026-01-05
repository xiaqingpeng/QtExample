#include "contenttab.h"
#include <QScrollArea>
#include <QGridLayout>

ContentTab::ContentTab(QWidget *parent)
    : QWidget(parent)
    , m_networkManager(new NetworkManager(this))
    , m_titleLabel(nullptr)
{
    setupUI();
    applyTheme();
}

ContentTab::~ContentTab()
{
}

void ContentTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 标题
    m_titleLabel = new QLabel("设备信息", this);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; padding: 10px 0;");
    mainLayout->addWidget(m_titleLabel);
    
    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(20);
    
    // 设备基本信息卡片
    QLabel *infoLabel = new QLabel("\n设备基本信息\n", this);
    infoLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px 0;");
    contentLayout->addWidget(infoLabel);
    
    // 创建信息网格
    QGridLayout *infoGrid = new QGridLayout();
    infoGrid->setSpacing(15);
    
    QStringList labels = {"设备名称", "设备类型", "操作系统", "CPU架构", "主机名", "IP地址", "MAC地址", "设备状态"};
    QStringList values = {"Qt6 Example Server", "开发服务器", "macOS", "x86_64", "localhost", "127.0.0.1", "00:00:00:00:00:00", "运行中"};
    
    for (int i = 0; i < labels.size(); ++i) {
        QLabel *label = new QLabel(labels[i] + ":", this);
        label->setStyleSheet("font-weight: bold; color: #666;");
        infoGrid->addWidget(label, i / 2, (i % 2) * 2);
        
        QLabel *value = new QLabel(values[i], this);
        value->setStyleSheet("color: #333;");
        infoGrid->addWidget(value, i / 2, (i % 2) * 2 + 1);
    }
    
    contentLayout->addLayout(infoGrid);
    contentLayout->addStretch();
    
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
}

void ContentTab::applyTheme()
{
    ThemeManager *theme = ThemeManager::instance();
    
    QString styleSheet = QString(
        "QWidget { "
        "    background-color: %1; "
        "    color: %2; "
        "    font-family: %3; "
        "}"
    ).arg(theme->colors().BACKGROUND)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY);
    
    setStyleSheet(styleSheet);
    
    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(QString(
            "QLabel { "
            "    font-size: %1px; "
            "    font-weight: bold; "
            "    color: %2; "
            "    padding: 10px 0; "
            "}"
        ).arg(ThemeManager::Typography::FONT_SIZE_XXL)
         .arg(theme->colors().TEXT_PRIMARY));
    }
}