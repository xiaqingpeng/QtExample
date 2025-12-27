#include "layoutexamplestab3.h"
#include <QDateEdit>
#include <QCalendarWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QTextEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QFrame>
#include <QHeaderView>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTabWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

LayoutExamplesTab3::LayoutExamplesTab3(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    
    // 初始化网络管理器
    networkManager = new NetworkManager(this);
    
    createDashboardLayout();
    createMultiPanelLayout();
    createCollapsiblePanelLayout();
    createResponsiveFormLayout();
    createApiDataTable();
}

void LayoutExamplesTab3::createDashboardLayout()
{
    QGroupBox *dashboardGroup = new QGroupBox("仪表盘布局 (数据可视化界面)");
    QVBoxLayout *dashboardLayout = new QVBoxLayout(dashboardGroup);
    dashboardLayout->setSpacing(12);
    
    // 顶部工具栏
    QWidget *toolbar = new QWidget();
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setSpacing(8);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    
    toolbarLayout->addWidget(new QPushButton("刷新数据"));
    toolbarLayout->addWidget(new QPushButton("导出报告"));
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(new QLabel("时间范围:"));
    toolbarLayout->addWidget(new QComboBox());
    toolbarLayout->addWidget(new QPushButton("设置"));
    
    dashboardLayout->addWidget(toolbar);
    
    // 数据卡片网格
    QWidget *cardsGrid = new QWidget();
    QGridLayout *cardsGridLayout = new QGridLayout(cardsGrid);
    cardsGridLayout->setSpacing(12);
    cardsGridLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建数据卡片
    QStringList cardTitles = {
        "总用户数",
        "今日新增",
        "活跃用户",
        "转化率"
    };
    
    QStringList cardValues = {
        "12,345",
        "+234",
        "8,987",
        "23.5%"
    };
    
    QList<QColor> cardColors = {
        QColor(33, 150, 243),   // 蓝色
        QColor(76, 175, 80),    // 绿色
        QColor(255, 152, 0),    // 橙色
        QColor(156, 39, 176)    // 紫色
    };
    
    for (int i = 0; i < 4; ++i) {
        QWidget *card = new QWidget();
        card->setStyleSheet(
            QString("background-color: white; border-radius: 8px; border-left: 4px solid %1; box-shadow: 0 2px 4px rgba(0,0,0,0.1);")
            .arg(cardColors[i].name())
        );
        
        QVBoxLayout *cardInnerLayout = new QVBoxLayout(card);
        cardInnerLayout->setSpacing(5);
        cardInnerLayout->setContentsMargins(15, 15, 15, 15);
        
        QLabel *title = new QLabel(cardTitles[i]);
        title->setStyleSheet("color: #666; font-size: 14px;");
        
        QLabel *value = new QLabel(cardValues[i]);
        value->setStyleSheet(
            QString("color: %1; font-size: 24px; font-weight: bold;")
            .arg(cardColors[i].name())
        );
        
        QProgressBar *progress = new QProgressBar();
        progress->setMinimum(0);
        progress->setMaximum(100);
        progress->setValue(50 + i * 10);
        progress->setStyleSheet(
            QString("QProgressBar { border: none; border-radius: 4px; background-color: #f5f5f5; text-align: center; }")
            + QString("QProgressBar::chunk { background-color: %1; border-radius: 4px; }")
            .arg(cardColors[i].name())
        );
        
        cardInnerLayout->addWidget(title);
        cardInnerLayout->addWidget(value);
        cardInnerLayout->addWidget(progress);
        
        int row = i / 2;
        int col = i % 2;
        cardsGridLayout->addWidget(card, row, col);
    }
    
    dashboardLayout->addWidget(cardsGrid);
    
    // 图表区域
    QWidget *chartsArea = new QWidget();
    QHBoxLayout *chartsLayout = new QHBoxLayout(chartsArea);
    chartsLayout->setSpacing(12);
    chartsLayout->setContentsMargins(0, 0, 0, 0);
    
    // 左图表
    QWidget *chart1 = new QWidget();
    chart1->setStyleSheet("background-color: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);");
    QVBoxLayout *chart1Layout = new QVBoxLayout(chart1);
    chart1Layout->setContentsMargins(15, 15, 15, 15);
    chart1Layout->addWidget(new QLabel("用户增长趋势"));
    QFrame *chart1Placeholder = new QFrame();
    chart1Placeholder->setMinimumHeight(200);
    chart1Placeholder->setStyleSheet("background-color: #f5f5f5; border-radius: 4px;");
    chart1Layout->addWidget(chart1Placeholder);
    
    // 右图表
    QWidget *chart2 = new QWidget();
    chart2->setStyleSheet("background-color: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);");
    QVBoxLayout *chart2Layout = new QVBoxLayout(chart2);
    chart2Layout->setContentsMargins(15, 15, 15, 15);
    chart2Layout->addWidget(new QLabel("用户分布"));
    QFrame *chart2Placeholder = new QFrame();
    chart2Placeholder->setMinimumHeight(200);
    chart2Placeholder->setStyleSheet("background-color: #f5f5f5; border-radius: 4px;");
    chart2Layout->addWidget(chart2Placeholder);
    
    chartsLayout->addWidget(chart1, 1);
    chartsLayout->addWidget(chart2, 1);
    
    dashboardLayout->addWidget(chartsArea);
    
    mainLayout->addWidget(dashboardGroup);
}

void LayoutExamplesTab3::createMultiPanelLayout()
{
    QGroupBox *multiPanelGroup = new QGroupBox("多面板布局 (复杂界面设计)");
    QVBoxLayout *multiPanelLayout = new QVBoxLayout(multiPanelGroup);
    multiPanelLayout->setSpacing(12);
    
    // 顶部标签页
    QTabWidget *tabWidget = new QTabWidget();
    
    // 标签页1：表格
    QTableWidget *tableWidget = new QTableWidget(5, 3);
    tableWidget->setHorizontalHeaderLabels({"ID", "名称", "状态"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabWidget->addTab(tableWidget, "数据表格");
    
    // 标签页2：树形结构
    QTreeWidget *treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabel("目录结构");
    QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget, {"根目录"});
    // QTreeWidgetItem *folder1 = new QTreeWidgetItem(root, {"文件夹1"});
    // QTreeWidgetItem *folder2 = new QTreeWidgetItem(root, {"文件夹2"});
    // QTreeWidgetItem *file1 = new QTreeWidgetItem(folder1, {"文件1.txt"});
    // QTreeWidgetItem *file2 = new QTreeWidgetItem(folder1, {"文件2.txt"});
    // QTreeWidgetItem *file3 = new QTreeWidgetItem(folder2, {"文件3.txt"});
    root->setExpanded(true);
    tabWidget->addTab(treeWidget, "目录树");
    
    multiPanelLayout->addWidget(tabWidget);
    
    // 底部分割面板
    QWidget *bottomPanels = new QWidget();
    QHBoxLayout *bottomPanelsLayout = new QHBoxLayout(bottomPanels);
    bottomPanelsLayout->setSpacing(12);
    bottomPanelsLayout->setContentsMargins(0, 0, 0, 0);
    
    // 左侧信息面板
    QWidget *infoPanel = new QWidget();
    infoPanel->setStyleSheet("background-color: #f5f5f5; border-radius: 8px;");
    QVBoxLayout *infoPanelLayout = new QVBoxLayout(infoPanel);
    infoPanelLayout->setContentsMargins(15, 15, 15, 15);
    infoPanelLayout->addWidget(new QLabel("信息面板"));
    infoPanelLayout->addWidget(new QTextEdit("这是一个信息展示面板，用于显示详细信息。"));
    
    // 右侧控制面板
    QWidget *controlPanel = new QWidget();
    controlPanel->setStyleSheet("background-color: #f5f5f5; border-radius: 8px;");
    QVBoxLayout *controlPanelLayout = new QVBoxLayout(controlPanel);
    controlPanelLayout->setContentsMargins(15, 15, 15, 15);
    controlPanelLayout->addWidget(new QLabel("控制面板"));
    controlPanelLayout->addWidget(new QPushButton("操作1"));
    controlPanelLayout->addWidget(new QPushButton("操作2"));
    controlPanelLayout->addWidget(new QPushButton("操作3"));
    
    bottomPanelsLayout->addWidget(infoPanel, 2);
    bottomPanelsLayout->addWidget(controlPanel, 1);
    
    multiPanelLayout->addWidget(bottomPanels);
    
    mainLayout->addWidget(multiPanelGroup);
}

void LayoutExamplesTab3::createCollapsiblePanelLayout()
{
    QGroupBox *collapsibleGroup = new QGroupBox("可折叠面板布局 (节省空间)");
    QVBoxLayout *collapsibleLayout = new QVBoxLayout(collapsibleGroup);
    collapsibleLayout->setSpacing(10);
    
    // 可折叠面板是一个复杂的组件，这里使用简单的实现
    // 在实际项目中，可以使用QToolBox或自定义组件
    
    for (int i = 0; i < 3; ++i) {
        QWidget *panel = new QWidget();
        panel->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e0e0e0;");
        QVBoxLayout *panelLayout = new QVBoxLayout(panel);
        panelLayout->setContentsMargins(0, 0, 0, 0);
        panelLayout->setSpacing(0);
        
        // 面板标题栏
        QWidget *titleBar = new QWidget();
        titleBar->setStyleSheet("background-color: #f5f5f5; padding: 10px 15px;");
        QHBoxLayout *titleBarLayout = new QHBoxLayout(titleBar);
        titleBarLayout->setContentsMargins(0, 0, 0, 0);
        titleBarLayout->addWidget(new QLabel(QString("面板 %1 - 可折叠内容").arg(i+1)));
        titleBarLayout->addStretch();
        QPushButton *toggleBtn = new QPushButton("▼");
        toggleBtn->setFixedSize(24, 24);
        toggleBtn->setStyleSheet("background-color: transparent; border: none; font-size: 10px;");
        titleBarLayout->addWidget(toggleBtn);
        
        // 面板内容
        QWidget *content = new QWidget();
        QVBoxLayout *contentLayout = new QVBoxLayout(content);
        contentLayout->setContentsMargins(15, 10, 15, 15);
        contentLayout->addWidget(new QLabel("这里是面板的详细内容，可以包含各种控件和信息。"));
        contentLayout->addWidget(new QPushButton("面板操作按钮"));
        
        panelLayout->addWidget(titleBar);
        panelLayout->addWidget(content);
        
        collapsibleLayout->addWidget(panel);
    }
    
    mainLayout->addWidget(collapsibleGroup);
}

void LayoutExamplesTab3::createResponsiveFormLayout()
{
    QGroupBox *responsiveFormGroup = new QGroupBox("响应式表单布局 (适配不同屏幕)");
    QVBoxLayout *responsiveFormLayout = new QVBoxLayout(responsiveFormGroup);
    responsiveFormLayout->setSpacing(12);
    
    // 响应式表单容器
    QWidget *formContainer = new QWidget();
    QGridLayout *formLayout = new QGridLayout(formContainer);
    formLayout->setSpacing(12);
    formLayout->setColumnStretch(0, 1);
    formLayout->setColumnStretch(1, 2);
    formLayout->setColumnStretch(2, 1);
    formLayout->setColumnStretch(3, 2);
    
    // 表单字段
    QStringList fieldLabels = {
        "姓名",
        "邮箱",
        "电话",
        "地址",
        "城市",
        "省份",
        "邮编",
        "备注"
    };
    
    for (int i = 0; i < fieldLabels.size(); ++i) {
        QLabel *label = new QLabel(fieldLabels[i]);
        QWidget *field;
        
        if (fieldLabels[i] == "备注") {
            field = new QTextEdit();
            formLayout->addWidget(label, i, 0, 1, 1, Qt::AlignTop);
            formLayout->addWidget(field, i, 1, 1, 3);
        } else {
            field = new QLineEdit();
            formLayout->addWidget(label, i, 2 * (i % 2), 1, 1, Qt::AlignRight);
            formLayout->addWidget(field, i, 2 * (i % 2) + 1, 1, 1);
        }
    }
    
    // 表单按钮
    QWidget *buttonsWidget = new QWidget();
    QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsWidget);
    buttonsLayout->setSpacing(10);
    buttonsLayout->setAlignment(Qt::AlignRight);
    buttonsLayout->addWidget(new QPushButton("重置"));
    buttonsLayout->addWidget(new QPushButton("保存"));
    
    formLayout->addWidget(buttonsWidget, fieldLabels.size(), 0, 1, 4);
    
    responsiveFormLayout->addWidget(formContainer);
    
    mainLayout->addWidget(responsiveFormGroup);
}

// 创建API数据表格并请求数据
void LayoutExamplesTab3::createApiDataTable()
{
    QGroupBox *apiDataGroup = new QGroupBox("系统通知列表 (API数据展示)");
    QVBoxLayout *apiLayout = new QVBoxLayout(apiDataGroup);
    
    // 创建表格
    apiDataTable = new QTableWidget();
    apiDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    apiDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    apiDataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    apiDataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 添加表格到布局
    apiLayout->addWidget(apiDataTable);
    
    // 使用NetworkManager发送GET请求，自动添加平台识别头
    networkManager->get("http://120.48.95.51:7001/system/notice/list",
                       [this](const QJsonObject &response) {
        // 成功回调
        if (response["code"].toInt() != 0) {
            QMessageBox::warning(this, "API返回错误", response["msg"].toString());
            return;
        }
        
        // 提取通知列表
        QJsonArray noticesArray = response["rows"].toArray();
        
        // 设置表格列数和标题
        QStringList headers;
        headers << "通知ID" << "通知标题" << "通知内容" << "创建人" << "创建时间" << "更新人" << "更新时间";
        apiDataTable->setColumnCount(headers.size());
        apiDataTable->setHorizontalHeaderLabels(headers);
        
        // 设置表格行数
        apiDataTable->setRowCount(noticesArray.size());
        
        // 填充表格数据
        for (int i = 0; i < noticesArray.size(); ++i) {
            QJsonObject notice = noticesArray[i].toObject();
            
            // 设置表格数据并垂直居中
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(notice["noticeId"].toInt()));
            item0->setTextAlignment(Qt::AlignVCenter);
            apiDataTable->setItem(i, 0, item0);
            
            QTableWidgetItem *item1 = new QTableWidgetItem(notice["noticeTitle"].toString());
            item1->setTextAlignment(Qt::AlignVCenter);
            apiDataTable->setItem(i, 1, item1);
            
            QTableWidgetItem *item2 = new QTableWidgetItem(notice["noticeContent"].toString());
            item2->setTextAlignment(Qt::AlignVCenter);
            apiDataTable->setItem(i, 2, item2);
            
            QTableWidgetItem *item3 = new QTableWidgetItem(notice["createBy"].toString());
            item3->setTextAlignment(Qt::AlignVCenter);
            apiDataTable->setItem(i, 3, item3);
            
            QTableWidgetItem *item4 = new QTableWidgetItem(notice["createTime"].toString());
            item4->setTextAlignment(Qt::AlignVCenter);
            apiDataTable->setItem(i, 4, item4);
            
            QTableWidgetItem *item5 = new QTableWidgetItem(notice["updateBy"].toString());
            item5->setTextAlignment(Qt::AlignVCenter);
            apiDataTable->setItem(i, 5, item5);
            
            QTableWidgetItem *item6 = new QTableWidgetItem(notice["updateTime"].toString());
            item6->setTextAlignment(Qt::AlignVCenter);
            apiDataTable->setItem(i, 6, item6);
        }
        
        // 调整第2列（通知内容）的宽度以适应内容
        apiDataTable->resizeColumnToContents(2);
    },
    [this](const QString &errorMsg) {
        // 错误回调
        QMessageBox::warning(this, "API请求错误", errorMsg);
    });
    
    mainLayout->addWidget(apiDataGroup);
}


