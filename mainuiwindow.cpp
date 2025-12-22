#include "mainuiwindow.h"
#include "advancedcontrols/advancedcontrolstab.h"
#include "basiccontrols/basiccontrolstab.h"
#include "datadisplay/datadisplaytab.h"
#include "dialogs/dialogstab.h"
#include "layoutexamples/layoutexamplestab.h"

MainUIWindow::MainUIWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Qt UI控件综合示例");
    resize(1000, 700);

    setupUI();
}

void MainUIWindow::setupUI()
{
    mainLayout = new QVBoxLayout(this);

    // 标题
    titleLabel = new QLabel("Qt 常用 UI 控件全集示例");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 18, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2c3e50; margin: 15px;");
    mainLayout->addWidget(titleLabel);

    // 选项卡
    tabWidget = new QTabWidget();
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setStyleSheet("QTabBar::tab { height: 80px; width: 30px; }");

    // 添加各个选项卡
    tabWidget->addTab(new BasicControlsTab(), "基本控件");
    tabWidget->addTab(new DataDisplayTab(), "数据显示");
    tabWidget->addTab(new AdvancedControlsTab(), "高级控件");
    tabWidget->addTab(new DialogsTab(), "对话框");
    tabWidget->addTab(new LayoutExamplesTab(), "布局示例");

    mainLayout->addWidget(tabWidget);

    // 状态栏
    statusBar = new QStatusBar();
    statusBar->showMessage("就绪 | Qt版本: " + QString(qVersion()));
    mainLayout->addWidget(statusBar);
}
