#include "layoutexamplestab.h"

LayoutExamplesTab::LayoutExamplesTab(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    
    createGridLayout();
    createFormLayout();
    createHorizontalSplitLayout();
    createVerticalSplitLayout();
    createHorizontalImageTextLayout();
    createVerticalImageTextLayout();
}

void LayoutExamplesTab::createGridLayout()
{
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
    mainLayout->addWidget(gridGroup);
}

void LayoutExamplesTab::createFormLayout()
{
    QGroupBox *formGroup = new QGroupBox("表单布局");
    QFormLayout *formLayout = new QFormLayout(formGroup);
    
    formLayout->addRow("用户名:", new QLineEdit());
    formLayout->addRow("密码:", new QLineEdit());
    formLayout->addRow("记住我:", new QCheckBox());
    formLayout->addRow("", new QPushButton("登录"));
    mainLayout->addWidget(formGroup);
}

void LayoutExamplesTab::createHorizontalSplitLayout()
{
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
    mainLayout->addWidget(hSplitGroup);
}

void LayoutExamplesTab::createVerticalSplitLayout()
{
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
    mainLayout->addWidget(vSplitGroup);
}

void LayoutExamplesTab::createHorizontalImageTextLayout()
{
    QGroupBox *hImageTextGroup = new QGroupBox("水平列表图文混排（使用本地图片）");
    QVBoxLayout *hImageTextLayout = new QVBoxLayout(hImageTextGroup);
    
    QScrollArea *hScrollArea = new QScrollArea();
    hScrollArea->setWidgetResizable(true);
    hScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    QWidget *hContentWidget = new QWidget();
    QHBoxLayout *hContentLayout = new QHBoxLayout(hContentWidget);
    hContentLayout->setAlignment(Qt::AlignLeft);
    hContentLayout->setSpacing(15);
    
    // 使用项目中的小马图片
    QStringList imageFiles = {
        "小马发红包.png",
        "小马吃棒棒糖.png",
        "小马抱福字.png",
        "小马拿红包.png",
        "小马拿金元宝.png",
        "小马送饺子.png"
    };
    
    QStringList titles = {
        "小马发红包",
        "小马吃棒棒糖",
        "小马抱福字",
        "小马拿红包",
        "小马拿金元宝",
        "小马送饺子"
    };
    
    QStringList descriptions = {
        "可爱的小马在发放红包",
        "小马正在享受美味的棒棒糖",
        "小马抱着福字，祝福大家新年快乐",
        "小马手里拿着红包，准备送给朋友",
        "小马抱着金元宝，象征着财富和好运",
        "小马正在送饺子，传递温暖和关爱"
    };
    
    for (int i = 0; i < 6; i++) {
        QWidget *itemWidget = new QWidget();
        itemWidget->setFixedSize(250, 300);
        itemWidget->setStyleSheet("background-color: white; border: 1px solid #e0e0e0; border-radius: 5px;");
        
        QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 10, 10, 10);
        
        QLabel *imageLabel = new QLabel();
        imageLabel->setFixedHeight(150);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet("border-radius: 3px;");
        
        // 使用Qt资源系统加载图片
        QString imagePath = QString(":/images/%1").arg(imageFiles[i]);
        QPixmap pixmap(imagePath);
        
        if (pixmap.isNull()) {
            QColor color = QColor::fromHsv(i * 60, 150, 230);
            imageLabel->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(color.name()));
            imageLabel->setText(QString("图片 %1\n(未找到资源图片)").arg(i+1));
        } else {
            pixmap = pixmap.scaledToHeight(140, Qt::SmoothTransformation);
            imageLabel->setPixmap(pixmap);
            imageLabel->setToolTip("资源图片: " + imagePath);
        }
        
        itemLayout->addWidget(imageLabel);
        
        QLabel *titleLabel = new QLabel(titles[i]);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
        itemLayout->addWidget(titleLabel);
        
        QLabel *descLabel = new QLabel(descriptions[i]);
        descLabel->setStyleSheet("color: #616161; font-size: 12px; margin-top: 5px;");
        descLabel->setWordWrap(true);
        itemLayout->addWidget(descLabel);
        
        QPushButton *viewButton = new QPushButton("查看详情");
        viewButton->setStyleSheet("background-color: #4fc3f7; color: white; border: none; padding: 5px; border-radius: 3px; margin-top: 10px;");
        itemLayout->addWidget(viewButton);
        
        hContentLayout->addWidget(itemWidget);
    }
    
    hScrollArea->setWidget(hContentWidget);
    hImageTextLayout->addWidget(hScrollArea);
    mainLayout->addWidget(hImageTextGroup);
}

void LayoutExamplesTab::createVerticalImageTextLayout()
{
    QGroupBox *vImageTextGroup = new QGroupBox("垂直列表图文混排（使用本地图片）");
    QVBoxLayout *vImageTextLayout = new QVBoxLayout(vImageTextGroup);
    
    QScrollArea *vScrollArea = new QScrollArea();
    vScrollArea->setWidgetResizable(true);
    
    QWidget *vContentWidget = new QWidget();
    QVBoxLayout *vContentLayout = new QVBoxLayout(vContentWidget);
    vContentLayout->setSpacing(10);
    
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
        QWidget *itemWidget = new QWidget();
        itemWidget->setMinimumHeight(150);
        itemWidget->setStyleSheet("background-color: white; border: 1px solid #e0e0e0; border-radius: 5px;");
        
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 10, 10, 10);
        
        QLabel *imageLabel = new QLabel();
        imageLabel->setFixedSize(120, 120);
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setStyleSheet("border-radius: 3px;");
        
        // 使用项目中的小马图片
        QStringList imageFiles = {
            "小马发红包.png",
            "小马吃棒棒糖.png",
            "小马抱福字.png",
            "小马拿红包.png",
            "小马拿金元宝.png",
            "小马送饺子.png"
        };
        // 使用Qt资源系统加载图片
        QString imagePath = QString(":/images/%1").arg(imageFiles[i]);
        QPixmap pixmap(imagePath);
        
        if (pixmap.isNull()) {
            QColor color = QColor::fromHsv(i * 60, 150, 230);
            imageLabel->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(color.name()));
            imageLabel->setText(QString("图片 %1\n(未找到资源图片)").arg(i+1));
        } else {
            pixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            imageLabel->setPixmap(pixmap);
            imageLabel->setToolTip("资源图片: " + imagePath);
        }
        
        itemLayout->addWidget(imageLabel);
        
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
        
        QHBoxLayout *metaLayout = new QHBoxLayout();
        metaLayout->addWidget(new QLabel(QString("%1分钟前").arg(30 - i*5)));
        metaLayout->addStretch();
        
        QPushButton *readButton = new QPushButton("阅读全文");
        readButton->setStyleSheet("background-color: #81c784; color: white; border: none; padding: 3px 8px; border-radius: 3px;");
        metaLayout->addWidget(readButton);
        
        textLayout->addLayout(metaLayout);
        textLayout->addStretch();
        
        itemLayout->addWidget(textWidget, 1);
        
        vContentLayout->addWidget(itemWidget);
    }
    
    vScrollArea->setWidget(vContentWidget);
    vImageTextLayout->addWidget(vScrollArea);
    
    QLabel *imageHint = new QLabel(QString("提示: 本示例使用Qt资源系统中的小马图片\n资源路径: :/images/\n包含图片: 小马发红包.png, 小马吃棒棒糖.png, 小马抱福字.png, 小马拿红包.png, 小马拿金元宝.png, 小马送饺子.png"));
    imageHint->setStyleSheet("color: #757575; font-size: 11px; margin-top: 5px;");
    imageHint->setWordWrap(true);
    vImageTextLayout->addWidget(imageHint);
    
    mainLayout->addWidget(vImageTextGroup);
}