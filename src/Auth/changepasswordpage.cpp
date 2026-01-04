#include "changepasswordpage.h"
#include "../Styles/theme_manager.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QCryptographicHash>
#include <QTimer>
#include "analytics/analytics.h"

ChangePasswordPage::ChangePasswordPage(QWidget *parent)
    : QWidget(parent)
    , m_networkManager(new NetworkManager(this))
{
    // 设置页面背景
    this->setObjectName("changePasswordPage");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 顶部装饰区域
    QWidget *topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(80);
    
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 0, 24, 0);
    
    // 标题标签
    QLabel *logoLabel = new QLabel("修改密码");
    logoLabel->setObjectName("logoLabel");
    topBarLayout->addStretch();
    topBarLayout->addWidget(logoLabel);
    topBarLayout->addStretch();
    
    mainLayout->addWidget(topBar);
    
    // 中心内容区域
    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(40, 40, 40, 40);
    centerLayout->setAlignment(Qt::AlignCenter);
    
    // 主卡片
    QWidget *cardWidget = new QWidget();
    cardWidget->setObjectName("changePasswordCard");
    cardWidget->setMaximumWidth(500);
    cardWidget->setMinimumWidth(400);
    
    QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setSpacing(24);
    cardLayout->setContentsMargins(40, 40, 40, 40);

    // 卡片标题
    QLabel *titleLabel = new QLabel("修改密码");
    titleLabel->setObjectName("changePasswordTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(titleLabel);

    // 表单区域
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(20);

    // 旧密码
    QLabel *oldPasswordLabel = new QLabel("旧密码");
    oldPasswordLabel->setObjectName("fieldLabel");
    formLayout->addWidget(oldPasswordLabel);
    
    m_oldPasswordEdit = new QLineEdit();
    m_oldPasswordEdit->setObjectName("oldPasswordEdit");
    m_oldPasswordEdit->setEchoMode(QLineEdit::Password);
    m_oldPasswordEdit->setPlaceholderText("请输入旧密码");
    m_oldPasswordEdit->setMinimumHeight(44);
    formLayout->addWidget(m_oldPasswordEdit);

    // 新密码
    QLabel *newPasswordLabel = new QLabel("新密码");
    newPasswordLabel->setObjectName("fieldLabel");
    formLayout->addWidget(newPasswordLabel);
    
    m_newPasswordEdit = new QLineEdit();
    m_newPasswordEdit->setObjectName("newPasswordEdit");
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordEdit->setPlaceholderText("请输入新密码（6-16位）");
    m_newPasswordEdit->setMinimumHeight(44);
    formLayout->addWidget(m_newPasswordEdit);

    // 确认密码
    QLabel *confirmPasswordLabel = new QLabel("确认密码");
    confirmPasswordLabel->setObjectName("fieldLabel");
    formLayout->addWidget(confirmPasswordLabel);
    
    m_confirmPasswordEdit = new QLineEdit();
    m_confirmPasswordEdit->setObjectName("confirmPasswordEdit");
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setPlaceholderText("请再次输入新密码");
    m_confirmPasswordEdit->setMinimumHeight(44);
    formLayout->addWidget(m_confirmPasswordEdit);

    cardLayout->addLayout(formLayout);

    // 修改密码按钮
    m_changePasswordBtn = new QPushButton("修改密码");
    m_changePasswordBtn->setObjectName("changePasswordButton");
    m_changePasswordBtn->setMinimumHeight(44);
    cardLayout->addWidget(m_changePasswordBtn);

    centerLayout->addWidget(cardWidget);
    mainLayout->addWidget(centerWidget);
    
    // 连接信号
    connect(m_changePasswordBtn, &QPushButton::clicked, this, &ChangePasswordPage::onChangePasswordClicked);
    
    // 连接主题变化信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, 
            this, &ChangePasswordPage::onThemeChanged);
    
    // 应用当前主题
    QTimer::singleShot(0, this, &ChangePasswordPage::applyTheme);
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

    // 发送修改密码请求

    // 使用NetworkManager发送POST请求，自动添加平台识别头、Token和Cookie
    m_networkManager->post("http://120.48.95.51:7001/user/change-password",
                           json,
                           [this](const QJsonObject &response) {
        // 成功回调
        int code = response["code"].toInt();
        QString message = response["msg"].toString();

        if (code == 0) {
            QMessageBox::information(this, "成功", "密码修改成功！");
            // 追踪密码修改成功事件
            Analytics::SDK::instance()->track("password_change_success", {
                {"event_type", "click"},
                {"page", "change_password_page"}
            });
            // 清空输入框
            m_oldPasswordEdit->clear();
            m_newPasswordEdit->clear();
            m_confirmPasswordEdit->clear();
            // 发送密码修改成功信号
            emit passwordChanged();
        } else {
            QMessageBox::warning(this, "失败", message);
            // 追踪密码修改失败事件
            Analytics::SDK::instance()->track("password_change_failed", {
                {"event_type", "error"},
                {"page", "change_password_page"},
                {"error_message", message}
            });
        }
    },
    [this](const QString &errorMsg) {
        // 错误回调
        // 修改密码失败
        QMessageBox::warning(this, "错误", "网络请求失败: " + errorMsg);
        // 追踪密码修改失败事件（网络错误）
        Analytics::SDK::instance()->track("password_change_failed", {
            {"event_type", "error"},
            {"page", "change_password_page"},
            {"error_message", errorMsg},
            {"error_type", "network_error"}
        });
    });
}


void ChangePasswordPage::applyTheme()
{
    ThemeManager* themeManager = ThemeManager::instance();
    const auto& colors = themeManager->colors();
    
    // 首先设置页面的基础背景色
    this->setStyleSheet(QString("QWidget#changePasswordPage { background-color: %1; }").arg(colors.BACKGROUND));
    
    // 应用主题到页面
    QString pageStyle = QString(
        "QWidget#changePasswordPage { "
        "    background-color: %1; "
        "} "
        "QWidget#topBar { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 %2, stop:1 %3); "
        "} "
        "QLabel#logoLabel { "
        "    color: white; "
        "    background-color: transparent; "
        "    font-family: %4; "
        "    font-size: %5px; "
        "    font-weight: 700; "
        "    border: none; "
        "    padding: 0px; "
        "    margin: 0px; "
        "} "
        "QWidget#changePasswordCard { "
        "    background-color: %6; "
        "    border: 1px solid %7; "
        "    border-radius: %8px; "
        "} "
        "QLabel#changePasswordTitle { "
        "    color: %9; "
        "    font-family: %10; "
        "    font-size: %11px; "
        "    font-weight: 700; "
        "    margin-bottom: %12px; "
        "} "
        "QLabel#fieldLabel { "
        "    color: %13; "
        "    font-family: %14; "
        "    font-size: %15px; "
        "    font-weight: 500; "
        "    margin-bottom: %16px; "
        "}"
    ).arg(colors.BACKGROUND)
     .arg(colors.PRIMARY)
     .arg(colors.PRIMARY_HOVER)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_XL)
     .arg(colors.CARD)
     .arg(colors.BORDER)
     .arg(ThemeManager::BorderRadius::LG)
     .arg(colors.TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_XXL)
     .arg(ThemeManager::Spacing::MD)
     .arg(colors.TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_MD)
     .arg(ThemeManager::Spacing::XS);
    
    // 应用输入框样式和按钮样式 - 使用主题管理器的样式
    QString inputStyle = themeManager->getInputStyle();
    QString buttonPrimaryStyle = themeManager->getButtonStyle("primary");
    
    // 应用所有样式
    this->setStyleSheet(pageStyle + inputStyle + buttonPrimaryStyle);
}

void ChangePasswordPage::onThemeChanged()
{
    applyTheme();
    
    // 强制重绘所有子控件以确保主题立即生效
    QList<QWidget*> allWidgets = this->findChildren<QWidget*>();
    for (QWidget* widget : allWidgets) {
        if (widget) {
            widget->update();
            widget->repaint();
        }
    }
    
    // 强制重绘整个页面
    update();
    repaint();
    
    // 延迟再次应用主题，确保完全生效
    QTimer::singleShot(10, this, [this]() {
        applyTheme();
        update();
        repaint();
    });
}