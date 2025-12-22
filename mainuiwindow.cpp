#include "mainuiwindow.h"
#include "advancedcontrols/advancedcontrolstab.h"
#include "basiccontrols/basiccontrolstab.h"
#include "datadisplay/datadisplaytab.h"
#include "dialogs/dialogstab.h"
#include "layoutexamples/layoutexamplestab.h"
#include <QFont>
#include <QListWidgetItem>
#include <QScrollArea>

MainUIWindow::MainUIWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Qt UI控件综合示例");
    resize(1200, 800);

    setupUI();
}

void MainUIWindow::setupUI()
{
    // 创建主布局（3列网格布局）
    mainLayout = new QGridLayout(this);

    // 标题（横跨3列）
    titleLabel = new QLabel("Qt 常用 UI 控件全集示例");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 18, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2c3e50; margin: 15px;");
    mainLayout->addWidget(titleLabel, 0, 0, 1, 3);

    // 一级菜单（左侧）
    setupMainMenu();
    mainLayout->addWidget(mainMenuList, 1, 0);

    // 二级菜单（中间）
    subMenuList = new QListWidget();
    subMenuList->setMaximumWidth(200);
    subMenuList->setStyleSheet(
        "QListWidget { border: none; background-color: #f8f9fa; }"
        "QListWidget::item { padding: 10px; border-bottom: 1px solid #e9ecef; }"
        "QListWidget::item:hover { background-color: #e3f2fd; }"
        "QListWidget::item:selected { background-color: #2196f3; color: white; }"
    );
    connect(subMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onSubMenuClicked);
    mainLayout->addWidget(subMenuList, 1, 1);

    // 内容区域（右侧）
    setupContent();
    mainLayout->addWidget(contentStack, 1, 2);

    // 状态栏（横跨3列）
    statusBar = new QStatusBar();
    statusBar->showMessage("就绪 | Qt版本: " + QString(qVersion()));
    mainLayout->addWidget(statusBar, 2, 0, 1, 3);

    // 设置布局间距
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 5);
    mainLayout->setRowStretch(1, 1);
}

void MainUIWindow::setupMainMenu()
{
    mainMenuList = new QListWidget();
    mainMenuList->setMaximumWidth(150);
    mainMenuList->setStyleSheet(
        "QListWidget { border: none; background-color: #343a40; }"
        "QListWidget::item { padding: 15px; color: white; border-bottom: 1px solid #495057; }"
        "QListWidget::item:hover { background-color: #495057; }"
        "QListWidget::item:selected { background-color: #007bff; }"
    );

    // 添加一级菜单
    new QListWidgetItem("控件示例", mainMenuList);
    new QListWidgetItem("布局示例", mainMenuList);
    new QListWidgetItem("对话框示例", mainMenuList);

    connect(mainMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onMainMenuClicked);

    // 默认选择第一个一级菜单
    mainMenuList->setCurrentRow(0);
}

void MainUIWindow::setupSubMenu(const QString &mainMenu)
{
    subMenuList->clear();

    if (mainMenu == "控件示例") {
        new QListWidgetItem("基本控件", subMenuList);
        new QListWidgetItem("高级控件", subMenuList);
        new QListWidgetItem("数据显示", subMenuList);
    } else if (mainMenu == "布局示例") {
        new QListWidgetItem("布局示例", subMenuList);
    } else if (mainMenu == "对话框示例") {
        new QListWidgetItem("对话框", subMenuList);
    }

    // 默认选择第一个二级菜单
    if (subMenuList->count() > 0) {
        subMenuList->setCurrentRow(0);
        // 触发二级菜单点击事件，显示对应的内容
        onSubMenuClicked(subMenuList->currentItem());
    }
}

void MainUIWindow::setupContent()
{
    contentStack = new QStackedWidget();
    contentStack->setStyleSheet("QStackedWidget { background-color: white; border: none; }");
}

void MainUIWindow::onMainMenuClicked(QListWidgetItem *item)
{
    QString mainMenu = item->text();
    setupSubMenu(mainMenu);
}

void MainUIWindow::onSubMenuClicked(QListWidgetItem *item)
{
    QString subMenu = item->text();

    // 清空内容区域
    while (contentStack->count() > 0) {
        QWidget *widget = contentStack->widget(0);
        contentStack->removeWidget(widget);
        widget->deleteLater();
    }

    // 根据二级菜单创建对应的内容
    QWidget *contentWidget = nullptr;
    if (subMenu == "基本控件") {
        contentWidget = new BasicControlsTab();
    } else if (subMenu == "高级控件") {
        contentWidget = new AdvancedControlsTab();
    } else if (subMenu == "数据显示") {
        contentWidget = new DataDisplayTab();
    } else if (subMenu == "布局示例") {
        contentWidget = new LayoutExamplesTab();
    } else if (subMenu == "对话框") {
        contentWidget = new DialogsTab();
    }

    // 如果创建了内容控件，添加到内容区域
    if (contentWidget) {
        // 将内容控件放在滚动区域中
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidget(contentWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        contentStack->addWidget(scrollArea);
        contentStack->setCurrentWidget(scrollArea);
    }
}
