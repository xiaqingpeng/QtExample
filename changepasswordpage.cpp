#include "changepasswordpage.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QCryptographicHash>

ChangePasswordPage::ChangePasswordPage(QWidget *parent)
    : QWidget(parent)
    , m_networkManager(new NetworkManager(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    // 标题
    QLabel *titleLabel = new QLabel("修改密码", this);
    QFont titleFont("Arial", 18, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #2c3e50; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel);

    // 旧密码
    QHBoxLayout *oldPasswordLayout = new QHBoxLayout();
    QLabel *oldPasswordLabel = new QLabel("旧密码:", this);
    oldPasswordLabel->setFixedWidth(100);
    m_oldPasswordEdit = new QLineEdit(this);
    m_oldPasswordEdit->setEchoMode(QLineEdit::Password);
    m_oldPasswordEdit->setPlaceholderText("请输入旧密码");
    m_oldPasswordEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ccc; border-radius: 4px; }");
    oldPasswordLayout->addWidget(oldPasswordLabel);
    oldPasswordLayout->addWidget(m_oldPasswordEdit);
    mainLayout->addLayout(oldPasswordLayout);

    // 新密码
    QHBoxLayout *newPasswordLayout = new QHBoxLayout();
    QLabel *newPasswordLabel = new QLabel("新密码:", this);
    newPasswordLabel->setFixedWidth(100);
    m_newPasswordEdit = new QLineEdit(this);
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordEdit->setPlaceholderText("请输入新密码");
    m_newPasswordEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ccc; border-radius: 4px; }");
    newPasswordLayout->addWidget(newPasswordLabel);
    newPasswordLayout->addWidget(m_newPasswordEdit);
    mainLayout->addLayout(newPasswordLayout);

    // 确认密码
    QHBoxLayout *confirmPasswordLayout = new QHBoxLayout();
    QLabel *confirmPasswordLabel = new QLabel("确认密码:", this);
    confirmPasswordLabel->setFixedWidth(100);
    m_confirmPasswordEdit = new QLineEdit(this);
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setPlaceholderText("请再次输入新密码");
    m_confirmPasswordEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #ccc; border-radius: 4px; }");
    confirmPasswordLayout->addWidget(confirmPasswordLabel);
    confirmPasswordLayout->addWidget(m_confirmPasswordEdit);
    mainLayout->addLayout(confirmPasswordLayout);

    // 修改密码按钮
    m_changePasswordBtn = new QPushButton("修改密码", this);
    m_changePasswordBtn->setFixedHeight(40);
    m_changePasswordBtn->setStyleSheet(
        "QPushButton { background-color: #007bff; color: white; border: none; padding: 10px; border-radius: 4px; font-size: 14px; }"
        "QPushButton:hover { background-color: #0056b3; }"
        "QPushButton:pressed { background-color: #004085; }"
    );
    mainLayout->addWidget(m_changePasswordBtn);

    // 连接按钮点击信号
    connect(m_changePasswordBtn, &QPushButton::clicked, this, &ChangePasswordPage::onChangePasswordClicked);

    // 添加弹性空间
    mainLayout->addStretch();
}

void ChangePasswordPage::onChangePasswordClicked()
{
    QString oldPassword = m_oldPasswordEdit->text().trimmed();
    QString newPassword = m_newPasswordEdit->text().trimmed();
    QString confirmPassword = m_confirmPasswordEdit->text().trimmed();

    // 验证输入
    if (oldPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入旧密码");
        return;
    }

    if (newPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入新密码");
        return;
    }

    if (confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请确认新密码");
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "提示", "两次输入的新密码不一致");
        return;
    }

    if (newPassword.length() < 6) {
        QMessageBox::warning(this, "提示", "新密码长度不能少于6位");
        return;
    }

    // 构建请求体
    QJsonObject json;
    json["oldPassword"] = oldPassword;
    json["newPassword"] = newPassword;
    json["confirmPassword"] = confirmPassword;

    qDebug() << "发送修改密码请求...";

    // 使用NetworkManager发送POST请求，自动添加平台识别头、Token和Cookie
    m_networkManager->post("http://120.48.95.51:7001/user/change-password",
                           json,
                           [this](const QJsonObject &response) {
        // 成功回调
        int code = response["code"].toInt();
        QString message = response["msg"].toString();

        if (code == 0) {
            QMessageBox::information(this, "成功", "密码修改成功！");
            // 清空输入框
            m_oldPasswordEdit->clear();
            m_newPasswordEdit->clear();
            m_confirmPasswordEdit->clear();
            // 发送密码修改成功信号
            emit passwordChanged();
        } else {
            QMessageBox::warning(this, "失败", message);
        }
    },
    [this](const QString &errorMsg) {
        // 错误回调
        qDebug() << "修改密码失败:" << errorMsg;
        QMessageBox::warning(this, "错误", "网络请求失败: " + errorMsg);
    });
}

