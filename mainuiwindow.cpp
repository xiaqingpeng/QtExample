#include "mainuiwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QListWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QDial>
#include <QLCDNumber>
#include <QFont>
#include <QMessageBox>
#include <QTableWidget>
#include <QTreeWidget>
#include <QToolButton>
#include <QScrollArea>
#include <QSplitter>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QFrame>
#include <QIcon>
#include <QHeaderView>
#include <QCalendarWidget>
#include <QGridLayout>
#include <QFormLayout>
#include <QSpacerItem>
#include <QPixmap>
#include <QDir>
#include <QStandardPaths>

MainUIWindow::MainUIWindow(QWidget *parent) : QWidget(parent) {
    // 设置主界面
    setWindowTitle("Qt UI控件综合示例");
    resize(1000, 700);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 创建内容容器
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    // 添加标题
    QLabel *titleLabel = new QLabel("Qt 常用 UI 控件全集示例");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 18, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2c3e50; margin: 15px;");
    contentLayout->addWidget(titleLabel);

    // 创建选项卡
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setStyleSheet("QTabBar::tab { height: 80px; width: 30px; }");
    contentLayout->addWidget(tabWidget);

    // ================= 第一个选项卡 - 基本控件 =================
    QWidget *basicTab = new QWidget();
    QVBoxLayout *basicLayout = new QVBoxLayout(basicTab);
    basicLayout->setSpacing(15);

    // 按钮组
    QGroupBox *buttonGroup = new QGroupBox("按钮控件");
    QGridLayout *buttonLayout = new QGridLayout(buttonGroup);

    QPushButton *standardBtn = new QPushButton("标准按钮");
    QPushButton *iconBtn = new QPushButton("图标按钮");
    iconBtn->setIcon(QIcon::fromTheme("document-open"));
    QPushButton *flatBtn = new QPushButton("扁平按钮");
    flatBtn->setFlat(true);
    QToolButton *toolBtn = new QToolButton();
    toolBtn->setText("工具按钮");
    toolBtn->setIcon(QIcon::fromTheme("edit-copy"));
    toolBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    buttonLayout->addWidget(standardBtn, 0, 0);
    buttonLayout->addWidget(iconBtn, 0, 1);
    buttonLayout->addWidget(flatBtn, 1, 0);
    buttonLayout->addWidget(toolBtn, 1, 1);
    basicLayout->addWidget(buttonGroup);

    // 输入控件组
    QGroupBox *inputGroup = new QGroupBox("输入控件");
    QFormLayout *inputLayout = new QFormLayout(inputGroup);

    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("单行文本输入...");
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setPlaceholderText("多行富文本输入...");
    QSpinBox *spinBox = new QSpinBox();
    spinBox->setRange(0, 100);
    QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();
    doubleSpinBox->setRange(0.0, 10.0);
    doubleSpinBox->setSingleStep(0.1);

    inputLayout->addRow("文本输入框:", lineEdit);
    inputLayout->addRow("富文本编辑:", textEdit);
    inputLayout->addRow("整数微调框:", spinBox);
    inputLayout->addRow("浮点微调框:", doubleSpinBox);
    basicLayout->addWidget(inputGroup);

    // 选择控件组
    QGroupBox *selectionGroup = new QGroupBox("选择控件");
    QGridLayout *selectionLayout = new QGridLayout(selectionGroup);

    QCheckBox *checkBox1 = new QCheckBox("选项1");
    QCheckBox *checkBox2 = new QCheckBox("选项2");
    QRadioButton *radio1 = new QRadioButton("单选1");
    QRadioButton *radio2 = new QRadioButton("单选2");
    radio1->setChecked(true);
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems({"选项A", "选项B", "选项C"});
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);

    selectionLayout->addWidget(checkBox1, 0, 0);
    selectionLayout->addWidget(checkBox2, 0, 1);
    selectionLayout->addWidget(radio1, 1, 0);
    selectionLayout->addWidget(radio2, 1, 1);
    selectionLayout->addWidget(new QLabel("下拉框:"), 2, 0);
    selectionLayout->addWidget(comboBox, 2, 1);
    selectionLayout->addWidget(new QLabel("滑块:"), 3, 0);
    selectionLayout->addWidget(slider, 3, 1);
    basicLayout->addWidget(selectionGroup);

    tabWidget->addTab(basicTab, "基本控件");

    // ================= 第二个选项卡 - 数据显示 =================
    QWidget *dataTab = new QWidget();
    QHBoxLayout *dataLayout = new QHBoxLayout(dataTab);

    // 列表控件
    QGroupBox *listGroup = new QGroupBox("列表控件");
    QVBoxLayout *listLayout = new QVBoxLayout(listGroup);

    QListWidget *listWidget = new QListWidget();
    listWidget->addItems({"项目1", "项目2", "项目3", "项目4"});
    listLayout->addWidget(listWidget);
    dataLayout->addWidget(listGroup);

    // 表格控件
    QGroupBox *tableGroup = new QGroupBox("表格控件");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);

    QTableWidget *tableWidget = new QTableWidget(5, 3);
    tableWidget->setHorizontalHeaderLabels({"姓名", "年龄", "职业"});
    tableWidget->verticalHeader()->setVisible(false);
    // 填充一些示例数据
    QStringList names = {"张三", "李四", "王五", "赵六", "钱七"};
    for (int row = 0; row < 5; ++row) {
        tableWidget->setItem(row, 0, new QTableWidgetItem(names[row]));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(25 + row)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(row % 2 == 0 ? "工程师" : "设计师"));
    }
    tableLayout->addWidget(tableWidget);
    dataLayout->addWidget(tableGroup);

    // 树形控件
    QGroupBox *treeGroup = new QGroupBox("树形控件");
    QVBoxLayout *treeLayout = new QVBoxLayout(treeGroup);

    QTreeWidget *treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabels({"名称", "类型"});
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(treeWidget, {"根节点", "文件夹"});
    new QTreeWidgetItem(rootItem, {"文档", "文件夹"});
    new QTreeWidgetItem(rootItem, {"图片", "文件夹"});
    QTreeWidgetItem *childItem = new QTreeWidgetItem(rootItem, {"项目", "文件夹"});
    new QTreeWidgetItem(childItem, {"main.cpp", "源代码"});
    new QTreeWidgetItem(childItem, {"widget.h", "头文件"});
    treeWidget->expandAll();
    treeLayout->addWidget(treeWidget);
    dataLayout->addWidget(treeGroup);

    tabWidget->addTab(dataTab, "数据显示");

    // ================= 第三个选项卡 - 高级控件 =================
    QWidget *advancedTab = new QWidget();
    QVBoxLayout *advancedLayout = new QVBoxLayout(advancedTab);

    // 进度显示
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
    advancedLayout->addWidget(progressGroup);

    // 日期时间
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
    advancedLayout->addWidget(datetimeGroup);

    // 分割视图
    QGroupBox *splitterGroup = new QGroupBox("分割视图");
    QVBoxLayout *splitterLayout = new QVBoxLayout(splitterGroup);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    QTextEdit *leftEdit = new QTextEdit("左侧区域 - 可以放置任何内容\n\n尝试拖动分割线调整大小");
    QTextEdit *rightEdit = new QTextEdit("右侧区域 - 分割视图非常实用\n\n适用于需要同时查看两个相关面板的场景");
    splitter->addWidget(leftEdit);
    splitter->addWidget(rightEdit);
    splitter->setSizes({200, 400});
    splitterLayout->addWidget(splitter);
    advancedLayout->addWidget(splitterGroup);

    tabWidget->addTab(advancedTab, "高级控件");

    // ================= 第四个选项卡 - 对话框 =================
    QWidget *dialogTab = new QWidget();
    QVBoxLayout *dialogLayout = new QVBoxLayout(dialogTab);
    dialogLayout->setAlignment(Qt::AlignTop);

    // 对话框按钮
    auto createDialogButton = [this](const QString &text, auto dialogFunc) {
        QPushButton *btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this, dialogFunc);
        return btn;
    };

    dialogLayout->addWidget(createDialogButton("打开消息对话框", [this]() {
        QMessageBox::information(this, "消息", "这是一个信息对话框");
    }));

    dialogLayout->addWidget(createDialogButton("打开文件对话框", [this]() {
        QFileDialog::getOpenFileName(this, "选择文件", QDir::homePath());
    }));

    dialogLayout->addWidget(createDialogButton("打开颜色对话框", [this]() {
        QColorDialog::getColor(Qt::red, this, "选择颜色");
    }));

    dialogLayout->addWidget(createDialogButton("打开字体对话框", [this]() {
        QFontDialog::getFont(nullptr, this);
    }));

    dialogLayout->addWidget(createDialogButton("输入对话框", [this]() {
        QInputDialog::getText(this, "输入", "请输入内容:");
    }));

    // 滚动区域示例
    QGroupBox *scrollGroup = new QGroupBox("滚动区域");
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollGroup);

    QScrollArea *demoScrollArea = new QScrollArea();
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *demoContentLayout = new QVBoxLayout(scrollContent);

    for (int i = 1; i <= 20; ++i) {
        QLabel *label = new QLabel("滚动内容项 #" + QString::number(i));
        label->setStyleSheet(QString("background-color: %1; padding: 8px;")
                             .arg(i % 2 == 0 ? "#f0f0f0" : "#ffffff"));
        demoContentLayout->addWidget(label);
    }

    demoScrollArea->setWidget(scrollContent);
    demoScrollArea->setWidgetResizable(true);
    scrollLayout->addWidget(demoScrollArea);
    dialogLayout->addWidget(scrollGroup);

    tabWidget->addTab(dialogTab, "对话框");

    // ================= 第五个选项卡 - 布局示例 =================
    QWidget *layoutTab = new QWidget();
    QVBoxLayout *layoutTabLayout = new QVBoxLayout(layoutTab);
    layoutTabLayout->setSpacing(15);

    // 网格布局
    QGroupBox *gridGroup = new QGroupBox("网格布局");
    QGridLayout *gridLayout = new QGridLayout(gridGroup);

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            QPushButton *btn = new QPushButton(
                QString("行%1列%2").arg(row+1).arg(col+1)
            );
            btn->setMinimumHeight(40);
            gridLayout->addWidget(btn, row, col);
        }
    }
    layoutTabLayout->addWidget(gridGroup);

    // 表单布局
    QGroupBox *formGroup = new QGroupBox("表单布局");
    QFormLayout *formLayout = new QFormLayout(formGroup);

    formLayout->addRow("用户名:", new QLineEdit());
    formLayout->addRow("密码:", new QLineEdit());
    formLayout->addRow("记住我:", new QCheckBox());
    formLayout->addRow("", new QPushButton("登录"));
    layoutTabLayout->addWidget(formGroup);

    // 水平两栏布局
    QGroupBox *hSplitGroup = new QGroupBox("水平两栏布局 (左侧固定, 右侧自适应)");
    QHBoxLayout *hSplitLayout = new QHBoxLayout(hSplitGroup);

    // 左侧固定区域
    QWidget *leftFixedWidget = new QWidget();
    leftFixedWidget->setFixedWidth(200);
    leftFixedWidget->setStyleSheet("background-color: #e3f2fd; border: 1px solid #bbdefb;");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftFixedWidget);
    leftLayout->addWidget(new QLabel("左侧固定区域"));
    leftLayout->addWidget(new QLabel("宽度: 200px"));
    leftLayout->addWidget(new QPushButton("操作按钮"));
    leftLayout->addStretch();

    // 右侧自适应区域
    QWidget *rightAutoWidget = new QWidget();
    rightAutoWidget->setStyleSheet("background-color: #fff8e1; border: 1px solid #ffecb3;");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightAutoWidget);
    rightLayout->addWidget(new QLabel("右侧自适应区域"));
    rightLayout->addWidget(new QLabel("宽度会随窗口调整"));
    QTextEdit *autoTextEdit = new QTextEdit("此区域会填充剩余空间\n尝试调整窗口大小查看效果");
    rightLayout->addWidget(autoTextEdit);

    hSplitLayout->addWidget(leftFixedWidget);
    hSplitLayout->addWidget(rightAutoWidget, 1);
    layoutTabLayout->addWidget(hSplitGroup);

    // 垂直两栏布局
    QGroupBox *vSplitGroup = new QGroupBox("垂直两栏布局 (顶部固定, 底部自适应)");
    QVBoxLayout *vSplitLayout = new QVBoxLayout(vSplitGroup);

    // 顶部固定区域
    QWidget *topFixedWidget = new QWidget();
    topFixedWidget->setFixedHeight(100);
    topFixedWidget->setStyleSheet("background-color: #f1f8e9; border: 1px solid #dcedc8;");
    QVBoxLayout *topLayout = new QVBoxLayout(topFixedWidget);
    topLayout->addWidget(new QLabel("顶部固定区域"));
    topLayout->addWidget(new QLabel("高度: 100px"));
    QHBoxLayout *buttonRow = new QHBoxLayout();
    buttonRow->addWidget(new QPushButton("确定"));
    buttonRow->addWidget(new QPushButton("取消"));
    topLayout->addLayout(buttonRow);

    // 底部自适应区域
    QWidget *bottomAutoWidget = new QWidget();
    bottomAutoWidget->setStyleSheet("background-color: #f3e5f5; border: 1px solid #e1bee7;");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomAutoWidget);
    bottomLayout->addWidget(new QLabel("底部自适应区域"));
    bottomLayout->addWidget(new QLabel("高度会随窗口调整"));
    QListWidget *autoList = new QListWidget();
    autoList->addItems({"项目1", "项目2", "项目3", "项目4", "项目5", "项目6", "项目7"});
    bottomLayout->addWidget(autoList);

    vSplitLayout->addWidget(topFixedWidget);
    vSplitLayout->addWidget(bottomAutoWidget, 1);
    layoutTabLayout->addWidget(vSplitGroup);

    // ===== 水平列表图文混排（使用本地图片） =====
    QGroupBox *hImageTextGroup = new QGroupBox("水平列表图文混排（使用本地图片）");
    QVBoxLayout *hImageTextLayout = new QVBoxLayout(hImageTextGroup);

    // 滚动区域容器
    QScrollArea *hScrollArea = new QScrollArea();
    hScrollArea->setWidgetResizable(true);
    hScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // 内容容器
    QWidget *hContentWidget = new QWidget();
    QHBoxLayout *hContentLayout = new QHBoxLayout(hContentWidget);
    hContentLayout->setAlignment(Qt::AlignLeft);
    hContentLayout->setSpacing(15);

    // 获取项目中的图片目录
    QString imageDir = QDir::currentPath() + "/images";
    // 如果当前目录没有images文件夹，尝试从项目根目录查找
    if (!QDir(imageDir).exists()) {
        imageDir = QDir::currentPath() + "/../images";
    }

    // 创建多个图文混排项目
    QStringList titles = {
        "美丽的自然风光",
        "城市夜景",
        "海滩度假",
        "山脉徒步",
        "历史建筑",
        "现代艺术"
    };

    QStringList descriptions = {
        "壮丽的山脉和湖泊，大自然的杰作",
        "灯火辉煌的城市夜景，繁华都市的魅力",
        "阳光沙滩，休闲度假的理想之地",
        "徒步旅行者的天堂，挑战自我的旅程",
        "古老建筑的魅力，历史的见证",
        "当代艺术的创新表达，视觉的盛宴"
    };

    // 尝试加载本地图片，如果失败则使用默认图片
    for (int i = 0; i < 6; i++) {
        // 创建单个项目容器
        QWidget *itemWidget = new QWidget();
        itemWidget->setFixedSize(250, 300);
        itemWidget->setStyleSheet("background-color: white; border: 1px solid #e0e0e0; border-radius: 5px;");

        QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 10, 10, 10);

        // 图片区域 - 使用本地图片
        QLabel *imageLabel = new QLabel();
        imageLabel->setFixedHeight(150);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet("border-radius: 3px;");

        // 尝试加载图片
        QString imagePath = QString("%1/example%2.png").arg(imageDir).arg(i+1);
        QPixmap pixmap(imagePath);

        if (pixmap.isNull()) {
            // 如果图片不存在，使用彩色占位符
            QColor color = QColor::fromHsv(i * 60, 150, 230);
            imageLabel->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(color.name()));
            imageLabel->setText(QString("图片 %1\n(未找到本地图片)").arg(i+1));
        } else {
            // 缩放图片并设置
            pixmap = pixmap.scaledToHeight(140, Qt::SmoothTransformation);
            imageLabel->setPixmap(pixmap);
            imageLabel->setToolTip("本地图片: " + imagePath);
        }

        itemLayout->addWidget(imageLabel);

        // 文字区域
        QLabel *titleLabel = new QLabel(titles[i]);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
        itemLayout->addWidget(titleLabel);

        QLabel *descLabel = new QLabel(descriptions[i]);
        descLabel->setStyleSheet("color: #616161; font-size: 12px; margin-top: 5px;");
        descLabel->setWordWrap(true);
        itemLayout->addWidget(descLabel);

        // 底部按钮
        QPushButton *viewButton = new QPushButton("查看详情");
        viewButton->setStyleSheet("background-color: #4fc3f7; color: white; border: none; padding: 5px; border-radius: 3px; margin-top: 10px;");
        itemLayout->addWidget(viewButton);

        // 添加到水平布局
        hContentLayout->addWidget(itemWidget);
    }

    hScrollArea->setWidget(hContentWidget);
    hImageTextLayout->addWidget(hScrollArea);
    layoutTabLayout->addWidget(hImageTextGroup);

    // ===== 垂直列表图文混排（使用本地图片） =====
    QGroupBox *vImageTextGroup = new QGroupBox("垂直列表图文混排（使用本地图片）");
    QVBoxLayout *vImageTextLayout = new QVBoxLayout(vImageTextGroup);

    // 滚动区域容器
    QScrollArea *vScrollArea = new QScrollArea();
    vScrollArea->setWidgetResizable(true);

    // 内容容器
    QWidget *vContentWidget = new QWidget();
    QVBoxLayout *vContentLayout = new QVBoxLayout(vContentWidget);
    vContentLayout->setSpacing(10);

    // 创建多个图文混排项目
    QStringList vTitles = {
        "科技新闻",
        "财经报道",
        "体育赛事",
        "娱乐八卦",
        "健康生活",
        "旅游攻略"
    };

    QStringList vDescriptions = {
        "最新科技动态：人工智能在医疗领域的突破性应用",
        "全球股市波动分析：投资者应该如何应对市场变化",
        "世界杯足球赛：冠军争夺战精彩回顾",
        "明星婚礼现场直击：豪华阵容羡煞旁人",
        "健康饮食指南：营养师推荐的10种超级食物",
        "东南亚旅游攻略：必去的10个景点和当地美食"
    };

    for (int i = 0; i < 6; i++) {
        // 创建单个项目容器
        QWidget *itemWidget = new QWidget();
        itemWidget->setMinimumHeight(150);
        itemWidget->setStyleSheet("background-color: white; border: 1px solid #e0e0e0; border-radius: 5px;");

        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 10, 10, 10);

        // 图片区域 - 使用本地图片
        QLabel *imageLabel = new QLabel();
        imageLabel->setFixedSize(120, 120);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet("border-radius: 3px;");

        // 尝试加载图片
        QString imagePath = QString("%1/example%2.png").arg(imageDir).arg(i+1);
        QPixmap pixmap(imagePath);

        if (pixmap.isNull()) {
            // 如果图片不存在，使用彩色占位符
            QColor color = QColor::fromHsv(i * 60, 150, 230);
            imageLabel->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(color.name()));
            imageLabel->setText(QString("图片 %1\n(未找到本地图片)").arg(i+1));
        } else {
            // 缩放图片并设置
            pixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            imageLabel->setPixmap(pixmap);
            imageLabel->setToolTip("本地图片: " + imagePath);
        }

        itemLayout->addWidget(imageLabel);

        // 文字区域
        QWidget *textWidget = new QWidget();
        QVBoxLayout *textLayout = new QVBoxLayout(textWidget);
        textLayout->setContentsMargins(10, 0, 0, 0);

        QLabel *titleLabel = new QLabel(vTitles[i]);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        textLayout->addWidget(titleLabel);

        QLabel *descLabel = new QLabel(vDescriptions[i]);
        descLabel->setStyleSheet("color: #616161; font-size: 12px; margin-top: 5px;");
        descLabel->setWordWrap(true);
        textLayout->addWidget(descLabel);

        // 时间和按钮区域
        QHBoxLayout *metaLayout = new QHBoxLayout();
        metaLayout->addWidget(new QLabel(QString("%1分钟前").arg(30 - i*5)));
        metaLayout->addStretch();

        QPushButton *readButton = new QPushButton("阅读全文");
        readButton->setStyleSheet("background-color: #81c784; color: white; border: none; padding: 3px 8px; border-radius: 3px;");
        metaLayout->addWidget(readButton);

        textLayout->addLayout(metaLayout);
        textLayout->addStretch();

        itemLayout->addWidget(textWidget, 1); // 设置拉伸因子

        // 添加到垂直布局
        vContentLayout->addWidget(itemWidget);
    }

    vScrollArea->setWidget(vContentWidget);
    vImageTextLayout->addWidget(vScrollArea);

    // 添加图片提示
    QLabel *imageHint = new QLabel(QString("提示: 本示例使用项目中的图片\n图片路径: %1/example[1-6].png\n如果未找到图片，将显示彩色占位符").arg(imageDir));
    imageHint->setStyleSheet("color: #757575; font-size: 11px; margin-top: 5px;");
    imageHint->setWordWrap(true);
    vImageTextLayout->addWidget(imageHint);

    layoutTabLayout->addWidget(vImageTextGroup);

    tabWidget->addTab(layoutTab, "布局示例");

    // 将内容容器设置到滚动区域
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);

    // 添加状态栏
    QStatusBar *statusBar = new QStatusBar();
    statusBar->showMessage("就绪 | Qt版本: " + QString(qVersion()));
    mainLayout->addWidget(statusBar);
}