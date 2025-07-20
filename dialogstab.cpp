#include "dialogstab.h"

DialogsTab::DialogsTab(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    
    createDialogButtons();
    createScrollArea();
}

void DialogsTab::createDialogButtons()
{
    QPushButton *messageBtn = new QPushButton("打开消息对话框");
    connect(messageBtn, &QPushButton::clicked, this, &DialogsTab::showMessageDialog);
    
    QPushButton *fileBtn = new QPushButton("打开文件对话框");
    connect(fileBtn, &QPushButton::clicked, this, &DialogsTab::showFileDialog);
    
    QPushButton *colorBtn = new QPushButton("打开颜色对话框");
    connect(colorBtn, &QPushButton::clicked, this, &DialogsTab::showColorDialog);
    
    QPushButton *fontBtn = new QPushButton("打开字体对话框");
    connect(fontBtn, &QPushButton::clicked, this, &DialogsTab::showFontDialog);
    
    QPushButton *inputBtn = new QPushButton("打开输入对话框");
    connect(inputBtn, &QPushButton::clicked, this, &DialogsTab::showInputDialog);
    
    mainLayout->addWidget(messageBtn);
    mainLayout->addWidget(fileBtn);
    mainLayout->addWidget(colorBtn);
    mainLayout->addWidget(fontBtn);
    mainLayout->addWidget(inputBtn);
}

void DialogsTab::createScrollArea()
{
    QGroupBox *scrollGroup = new QGroupBox("滚动区域");
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollGroup);
    
    QScrollArea *scrollArea = new QScrollArea();
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    
    for (int i = 1; i <= 20; ++i) {
        QLabel *label = new QLabel("滚动内容项 #" + QString::number(i));
        label->setStyleSheet(QString("background-color: %1; padding: 8px;")
                             .arg(i % 2 == 0 ? "#f0f0f0" : "#ffffff"));
        contentLayout->addWidget(label);
    }
    
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    scrollLayout->addWidget(scrollArea);
    mainLayout->addWidget(scrollGroup);
}

void DialogsTab::showMessageDialog()
{
    QMessageBox::information(this, "提示", "这是一个信息对话框");
}

void DialogsTab::showFileDialog()
{
    QFileDialog::getOpenFileName(this, "选择文件", QDir::homePath());
}

void DialogsTab::showColorDialog()
{
    QColorDialog::getColor(Qt::red, this, "选择颜色");
}

void DialogsTab::showFontDialog()
{
    QFontDialog::getFont(nullptr, this);
}

void DialogsTab::showInputDialog()
{
    QInputDialog::getText(this, "输入", "请输入内容:");
}