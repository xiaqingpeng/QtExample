#include "loginwindow.h"
#include "mainuiwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFont>


LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    // 设置窗口属性
    setWindowTitle("QQ登录");
    setFixedSize(400, 300);

    // 主布局
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 添加QQ图标
    auto *logoLabel = new QLabel(this);
    logoLabel->setText("QQ登录");
    logoLabel->setAlignment(Qt::AlignCenter);
    QFont logoFont = logoLabel->font();
    logoFont.setPointSize(20);
    logoLabel->setFont(logoFont);
    mainLayout->addWidget(logoLabel);

    // 账号输入框
    auto *accountLayout = new QHBoxLayout();
    auto *accountLabel = new QLabel("账号:", this);
    accountEdit = new QLineEdit(this);
    accountEdit->setPlaceholderText("QQ号码/手机/邮箱");
    accountEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ccc; border-radius: 3px; }");
    accountLayout->addWidget(accountLabel);
    accountLayout->addWidget(accountEdit);
    mainLayout->addLayout(accountLayout);

    // 密码输入框
    auto *passwordLayout = new QHBoxLayout();
    auto *passwordLabel = new QLabel("密码:", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ccc; border-radius: 3px; }");
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    mainLayout->addLayout(passwordLayout);

    // 选项布局
    auto *optionsLayout = new QHBoxLayout();
    rememberCheck = new QCheckBox("记住密码", this);
    autoLoginCheck = new QCheckBox("自动登录", this);
    optionsLayout->addWidget(rememberCheck);
    optionsLayout->addWidget(autoLoginCheck);
    optionsLayout->addStretch();
    mainLayout->addLayout(optionsLayout);

    // 登录按钮
    loginButton = new QPushButton("登录", this);
    loginButton->setStyleSheet(
        "QPushButton { background-color: #12B7F5; color: white; padding: 10px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #0E9DDB; }"
        "QPushButton:pressed { background-color: #0C8BC4; }"
        );
    loginButton->setCursor(Qt::PointingHandCursor);
    mainLayout->addWidget(loginButton);

    // 底部链接
    auto *bottomLayout = new QHBoxLayout();
    auto *registerBtn = new QPushButton("注册账号", this);
    auto *forgotBtn = new QPushButton("找回密码", this);
    registerBtn->setStyleSheet("QPushButton { border: none; color: #12B7F5; }");
    forgotBtn->setStyleSheet("QPushButton { border: none; color: #12B7F5; }");
    registerBtn->setCursor(Qt::PointingHandCursor);
    forgotBtn->setCursor(Qt::PointingHandCursor);
    bottomLayout->addWidget(registerBtn);
    bottomLayout->addStretch();
    bottomLayout->addWidget(forgotBtn);
    mainLayout->addLayout(bottomLayout);

    // 连接信号槽
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(forgotBtn, &QPushButton::clicked, this, &LoginWindow::onForgotClicked);
}

void LoginWindow::onLoginClicked() {
    QString account = accountEdit->text();
    QString password = passwordEdit->text();

    if (account.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "账号和密码不能为空");
        return;
    }

    // 模拟登录成功
    this->hide();
    auto *mainWindow = new MainUIWindow();
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    mainWindow->show();
}

void LoginWindow::onRegisterClicked() {
    QMessageBox::information(this, "注册账号", "跳转到QQ注册页面");
}

void LoginWindow::onForgotClicked() {
    QMessageBox::information(this, "找回密码", "跳转到密码找回页面");
}
