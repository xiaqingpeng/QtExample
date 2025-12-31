#include "layoutexamplestab2.h"

LayoutExamplesTab2::LayoutExamplesTab2(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    createComplexGridLayout();
    createNestedLayout();
    createResponsiveLayout();
    createCardLayout();
}

void LayoutExamplesTab2::createComplexGridLayout()
{
    QGroupBox *groupBox = new QGroupBox("复杂网格布局");
    QGridLayout *gridLayout = new QGridLayout(groupBox);
    
    // 创建多个控件并放置在不同的网格位置，使用不同的行列跨度
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 4; ++col) {
            QPushButton *button = new QPushButton(QString("(%1,%2)").arg(row).arg(col));
            button->setFixedSize(80, 40);
            
            // 为特定位置设置不同的行列跨度
            int rowSpan = 1;
            int colSpan = 1;
            
            if (row == 0 && col == 0) {
                rowSpan = 2;
                colSpan = 2;
            } else if (row == 2 && col == 1) {
                rowSpan = 1;
                colSpan = 2;
            } else if (row == 3 && col == 0) {
                rowSpan = 2;
                colSpan = 1;
            } else if (row == 3 && col == 2) {
                rowSpan = 1;
                colSpan = 2;
            }
            
            gridLayout->addWidget(button, row, col, rowSpan, colSpan);
        }
    }
    
    gridLayout->setSpacing(10);
    groupBox->setFixedSize(400, 300);
    mainLayout->addWidget(groupBox);
}

void LayoutExamplesTab2::createNestedLayout()
{
    QGroupBox *groupBox = new QGroupBox("嵌套布局");
    QVBoxLayout *outerLayout = new QVBoxLayout(groupBox);
    
    // 创建标题行
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("表单标题");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setFixedSize(60, 30);
    
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeButton);
    
    // 创建表单部分
    QFormLayout *formLayout = new QFormLayout();
    
    // 第一行：包含两个输入框的水平布局
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLineEdit("姓"));
    nameLayout->addWidget(new QLineEdit("名"));
    
    formLayout->addRow("姓名:", nameLayout);
    formLayout->addRow("邮箱:", new QLineEdit());
    
    // 地址部分：包含多行输入框的垂直布局
    QVBoxLayout *addressLayout = new QVBoxLayout();
    addressLayout->addWidget(new QLineEdit("街道"));
    addressLayout->addWidget(new QLineEdit("城市"));
    addressLayout->addWidget(new QLineEdit("邮编"));
    
    formLayout->addRow("地址:", addressLayout);
    
    // 创建按钮行
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(new QPushButton("保存"));
    buttonLayout->addWidget(new QPushButton("取消"));
    buttonLayout->setSpacing(10);
    
    // 将所有布局添加到外层布局
    outerLayout->addLayout(titleLayout);
    outerLayout->addLayout(formLayout);
    outerLayout->addLayout(buttonLayout);
    outerLayout->setSpacing(20);
    
    groupBox->setFixedSize(400, 300);
    mainLayout->addWidget(groupBox);
}

void LayoutExamplesTab2::createResponsiveLayout()
{
    QGroupBox *groupBox = new QGroupBox("响应式布局");
    QVBoxLayout *outerLayout = new QVBoxLayout(groupBox);
    
    // 创建一个包含三个按钮的水平布局，使用不同的伸缩因子
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *button1 = new QPushButton("伸缩因子: 1");
    QPushButton *button2 = new QPushButton("伸缩因子: 2");
    QPushButton *button3 = new QPushButton("伸缩因子: 1");
    
    buttonLayout->addWidget(button1, 1);
    buttonLayout->addWidget(button2, 2);
    buttonLayout->addWidget(button3, 1);
    
    // 创建一个包含文本区域和列表的水平布局，使用伸缩因子
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setPlaceholderText("文本区域 - 伸缩因子: 2");
    QListWidget *listWidget = new QListWidget();
    listWidget->setFixedWidth(150);
    listWidget->addItem("列表项1");
    listWidget->addItem("列表项2");
    listWidget->addItem("列表项3");
    
    contentLayout->addWidget(textEdit, 2);
    contentLayout->addWidget(listWidget);
    
    // 创建一个包含两个按钮的水平布局，右对齐
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addStretch();
    actionLayout->addWidget(new QPushButton("确定"));
    actionLayout->addWidget(new QPushButton("取消"));
    
    // 将所有布局添加到外层布局
    outerLayout->addLayout(buttonLayout);
    outerLayout->addLayout(contentLayout);
    outerLayout->addLayout(actionLayout);
    outerLayout->setSpacing(20);
    
    groupBox->setFixedSize(600, 400);
    mainLayout->addWidget(groupBox);
}

void LayoutExamplesTab2::createCardLayout()
{
    QGroupBox *groupBox = new QGroupBox("卡片布局");
    QHBoxLayout *outerLayout = new QHBoxLayout(groupBox);
    outerLayout->setSpacing(20);
    
    // 创建四张卡片
    for (int i = 0; i < 4; ++i) {
        QWidget *card = new QWidget();
        card->setStyleSheet("background-color: white; border-radius: 8px;");
        
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(10);
        cardLayout->setContentsMargins(15, 15, 15, 15);
        
        // 卡片标题
        QLabel *title = new QLabel(QString("卡片 %1").arg(i+1));
        title->setStyleSheet("font-weight: bold; font-size: 14px;");
        
        // 卡片内容
        QLabel *content = new QLabel("这是卡片的内容区域，包含一些描述性文本和其他UI元素。");
        content->setWordWrap(true);
        content->setStyleSheet("color: #666;");
        
        // 卡片按钮
        QPushButton *button = new QPushButton("了解更多");
        button->setStyleSheet("background-color: #2196F3; color: white; border: none; padding: 5px 15px; border-radius: 4px;");
        
        // 将元素添加到卡片布局
        cardLayout->addWidget(title);
        cardLayout->addWidget(content);
        cardLayout->addStretch();
        cardLayout->addWidget(button, 0, Qt::AlignRight);
        
        // 设置卡片宽度
        card->setFixedWidth(200);
        
        // 添加到外层布局
        outerLayout->addWidget(card);
    }
    
    groupBox->setFixedSize(900, 200);
    mainLayout->addWidget(groupBox);
}