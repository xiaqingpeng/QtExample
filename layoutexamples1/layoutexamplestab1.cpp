#include "layoutexamplestab1.h"
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
    formLayout->setVerticalSpacing(10);
    
    // 用户名
    QLineEdit *usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("请输入用户名");
    formLayout->addRow("用户名:", usernameEdit);
    
    // 密码
    QLineEdit *passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("密码:", passwordEdit);
    
    // 邮箱地址
    QLineEdit *emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("请输入邮箱地址");
    // 使用正则表达式验证邮箱格式，而不是输入掩码
    QRegularExpression emailRegExp("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}");
    emailEdit->setValidator(new QRegularExpressionValidator(emailRegExp, this));
    formLayout->addRow("邮箱地址:", emailEdit);
    
    // 手机号
    QLineEdit *phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("请输入手机号");
    phoneEdit->setInputMask("+86 999 9999 9999;_");
    formLayout->addRow("手机号:", phoneEdit);
    
    // 性别选择
    QWidget *genderWidget = new QWidget();
    QHBoxLayout *genderLayout = new QHBoxLayout(genderWidget);
    genderLayout->setContentsMargins(0, 0, 0, 0);
    
    QRadioButton *maleRadio = new QRadioButton("男");
    QRadioButton *femaleRadio = new QRadioButton("女");
    QRadioButton *otherRadio = new QRadioButton("其他");
    
    QButtonGroup *genderGroup = new QButtonGroup(genderWidget);
    genderGroup->addButton(maleRadio);
    genderGroup->addButton(femaleRadio);
    genderGroup->addButton(otherRadio);
    
    genderLayout->addWidget(maleRadio);
    genderLayout->addWidget(femaleRadio);
    genderLayout->addWidget(otherRadio);
    genderLayout->addStretch();
    
    formLayout->addRow("性别:", genderWidget);
    
    // 兴趣爱好
    QWidget *hobbyWidget = new QWidget();
    QHBoxLayout *hobbyLayout = new QHBoxLayout(hobbyWidget);
    hobbyLayout->setContentsMargins(0, 0, 0, 0);
    
    QCheckBox *readingCheck = new QCheckBox("阅读");
    QCheckBox *sportsCheck = new QCheckBox("运动");
    QCheckBox *musicCheck = new QCheckBox("音乐");
    QCheckBox *travelCheck = new QCheckBox("旅行");
    
    hobbyLayout->addWidget(readingCheck);
    hobbyLayout->addWidget(sportsCheck);
    hobbyLayout->addWidget(musicCheck);
    hobbyLayout->addWidget(travelCheck);
    hobbyLayout->addStretch();
    
    formLayout->addRow("兴趣爱好:", hobbyWidget);
    
    // 出生日期
    QDateEdit *birthdateEdit = new QDateEdit();
    birthdateEdit->setCalendarPopup(true);
    birthdateEdit->setDate(QDate::currentDate().addYears(-20));
    birthdateEdit->setDisplayFormat("yyyy-MM-dd");
    formLayout->addRow("出生日期:", birthdateEdit);
    
    // 所在城市
    QComboBox *cityCombo = new QComboBox();
    cityCombo->addItems({"北京", "上海", "广州", "深圳", "杭州", "成都", "武汉", "西安"});
    cityCombo->setEditable(true);
    formLayout->addRow("所在城市:", cityCombo);
    
    // 个人简介
    QTextEdit *bioEdit = new QTextEdit();
    bioEdit->setPlaceholderText("请输入个人简介...");
    bioEdit->setMaximumHeight(80);
    formLayout->addRow("个人简介:", bioEdit);
    
    // 同意协议
    QCheckBox *agreeCheck = new QCheckBox("我已阅读并同意用户协议和隐私政策");
    formLayout->addRow("", agreeCheck);
    
    // 记住我
    QCheckBox *rememberCheck = new QCheckBox("记住我");
    formLayout->addRow("", rememberCheck);
    
    // 按钮组
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    
    QPushButton *loginButton = new QPushButton("登录");
    loginButton->setStyleSheet("background-color: #2196F3; color: white; padding: 8px 16px; border-radius: 4px;");
    QPushButton *registerButton = new QPushButton("注册");
    registerButton->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px 16px; border-radius: 4px;");
    QPushButton *resetButton = new QPushButton("重置");
    resetButton->setStyleSheet("background-color: #FF9800; color: white; padding: 8px 16px; border-radius: 4px;");
    
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    
    formLayout->addRow("", buttonWidget);
    
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
        "pony_red_envelope.png",
        "pony_lollipop.png",
        "pony_fu_character.png",
        "pony_holding_envelope.png",
        "pony_gold_ingot.png",
        "pony_dumplings.png"
    };
    
    QStringList titles = {
        "Pony Red Envelope",
        "Pony Lollipop",
        "Pony Fu Character",
        "Pony Holding Envelope",
        "Pony Gold Ingot",
        "Pony Dumplings"
    };
    
    QStringList descriptions = {
        "Cute pony giving out red envelopes",
        "Pony enjoying delicious lollipop",
        "Pony Fu character, wishing everyone a happy New Year",
        "Pony holding red envelope, ready to give to friends",
        "Pony holding gold ingot, symbolizing wealth and good luck",
        "Pony delivering dumplings, spreading warmth and care"
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
            "pony_red_envelope.png",
            "pony_lollipop.png",
            "pony_fu_character.png",
            "pony_holding_envelope.png",
            "pony_gold_ingot.png",
            "pony_dumplings.png"
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
    
    QLabel *imageHint = new QLabel(QString("提示: 本示例使用Qt资源系统中的小马图片\n资源路径: :/images/\n包含图片: pony_red_envelope.png, pony_lollipop.png, pony_fu_character.png, pony_holding_envelope.png, pony_gold_ingot.png, pony_dumplings.png"));
    imageHint->setStyleSheet("color: #757575; font-size: 11px; margin-top: 5px;");
    imageHint->setWordWrap(true);
    vImageTextLayout->addWidget(imageHint);
    
    mainLayout->addWidget(vImageTextGroup);
}