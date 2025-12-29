#include "userinfopage.h"
#include <QSettings>
#include <QPixmap>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDateTime>

UserInfoPage::UserInfoPage(QWidget *parent)
    : QWidget(parent)
    , m_networkManager(new NetworkManager(this))
{
    setupUI();
    loadUserInfo();
}

UserInfoPage::~UserInfoPage()
{
}

void UserInfoPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 设置页面渐变背景
    this->setStyleSheet(
        "QWidget { "
        "    background: qlinearg-gradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "}"
    );
    
    // 顶部装饰区域
    QWidget *headerWidget = new QWidget();
    headerWidget->setFixedHeight(200);
    headerWidget->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 rgba(255,255,255,0.1), stop:1 rgba(255,255,255,0.05)); "
        "}"
    );
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    // 标题
    QLabel *titleLabel = new QLabel("个人中心");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 32px; "
        "    font-weight: bold; "
        "    color: white; "
        "    padding: 40px 0 20px 0; "
        "    letter-spacing: 2px; "
        "}"
    );
    headerLayout->addWidget(titleLabel);
    
    // 副标题
    QLabel *subtitleLabel = new QLabel("查看和管理您的个人信息");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    color: rgba(255,255,255,0.8); "
        "    padding: 0 0 20px 0; "
        "}"
    );
    headerLayout->addWidget(subtitleLabel);
    
    headerLayout->addStretch();
    mainLayout->addWidget(headerWidget);
    
    // 内容区域容器
    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("background-color: transparent;");
    
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 0, 40, 40);
    contentLayout->setSpacing(20);
    
    // 信息卡片
    QWidget *infoCard = new QWidget();
    infoCard->setStyleSheet(
        "QWidget { "
        "    background-color: white; "
        "    border-radius: 20px; "
        "    padding: 30px; "
        "}"
    );
    
    QVBoxLayout *cardLayout = new QVBoxLayout(infoCard);
    cardLayout->setSpacing(25);
    
    // 头像容器
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setFixedSize(140, 140);
    avatarContainer->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    border-radius: 70px; "
        "}"
    );
    
    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(0, 0, 0, 0);
    
    m_avatarLabel = new QLabel();
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setFixedSize(120, 120);
    m_avatarLabel->setStyleSheet(
        "QLabel { "
        "    background-color: white; "
        "    border-radius: 60px; "
        "    border: 4px solid rgba(255,255,255,0.3); "
        "}"
    );
    
    // 设置默认头像
    QPixmap defaultAvatar(120, 120);
    defaultAvatar.fill(QColor("#e9ecef"));
    m_avatarLabel->setPixmap(defaultAvatar);
    
    avatarLayout->addWidget(m_avatarLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(avatarContainer, 0, Qt::AlignCenter);
    
    // 用户名
    m_usernameLabel = new QLabel("加载中...");
    m_usernameLabel->setAlignment(Qt::AlignCenter);
    m_usernameLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 28px; "
        "    font-weight: bold; "
        "    color: #2c3e50; "
        "    padding: 10px; "
        "}"
    );
    cardLayout->addWidget(m_usernameLabel);
    
    // 装饰分隔线
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setFixedHeight(2);
    line->setStyleSheet(
        "QFrame { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    border-radius: 1px; "
        "}"
    );
    cardLayout->addWidget(line);
    
    // 信息项容器
    QWidget *infoItemsWidget = new QWidget();
    QVBoxLayout *infoItemsLayout = new QVBoxLayout(infoItemsWidget);
    infoItemsLayout->setSpacing(15);
    
    // 用户ID
    QWidget *idWidget = createInfoItem("用户ID", "加载中...");
    m_idLabel = idWidget->findChild<QLabel*>("valueLabel");
    infoItemsLayout->addWidget(idWidget);
    
    // 邮箱
    QWidget *emailWidget = createInfoItem("邮箱", "加载中...");
    m_emailLabel = emailWidget->findChild<QLabel*>("valueLabel");
    infoItemsLayout->addWidget(emailWidget);
    
    // 创建时间
    QWidget *timeWidget = createInfoItem("注册时间", "加载中...");
    m_createTimeLabel = timeWidget->findChild<QLabel*>("valueLabel");
    infoItemsLayout->addWidget(timeWidget);
    
    cardLayout->addWidget(infoItemsWidget);
    cardLayout->addStretch();
    contentLayout->addWidget(infoCard);
    contentLayout->addStretch();
    mainLayout->addWidget(contentWidget);
}

void UserInfoPage::loadUserInfo()
{
    // 从设置中获取用户信息
    QSettings settings("YourCompany", "QtApp");
    QString userId = settings.value("user/id", "").toString();
    QString username = settings.value("user/username", "").toString();
    QString email = settings.value("user/email", "").toString();
    QString avatar = settings.value("user/avatar", "").toString();
    QString createTime = settings.value("user/createTime", "").toString();
    
    qDebug() << "=== Loading User Info from Settings ===";
    qDebug() << "User ID:" << userId;
    qDebug() << "Username:" << username;
    qDebug() << "Email:" << email;
    qDebug() << "Avatar:" << avatar;
    qDebug() << "Create Time:" << createTime;
    
    if (userId.isEmpty() && username.isEmpty() && email.isEmpty()) {
        showError("未找到登录信息，请重新登录");
        return;
    }
    
    // 更新UI
    m_usernameLabel->setText(username.isEmpty() ? "未知用户" : username);
    m_idLabel->setText(userId.isEmpty() ? "未知" : userId);
    m_emailLabel->setText(email.isEmpty() ? "未知" : email);
    
    // 格式化创建时间
    if (!createTime.isEmpty()) {
        QDateTime utcTime = QDateTime::fromString(createTime, Qt::ISODate);
        if (utcTime.isValid()) {
            QDateTime beijingTime = utcTime.toOffsetFromUtc(8 * 3600);
            QString formattedTime = beijingTime.toString("yyyy-MM-dd HH:mm:ss");
            m_createTimeLabel->setText(formattedTime);
        } else {
            m_createTimeLabel->setText(createTime);
        }
    } else {
        m_createTimeLabel->setText("未知");
    }
    
    // 加载头像
    if (!avatar.isEmpty()) {
        QNetworkAccessManager *networkMgr = new QNetworkAccessManager(this);
        QNetworkRequest request{QUrl(avatar)};
        QNetworkReply *reply = networkMgr->get(request);
        
        connect(reply, &QNetworkReply::finished, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray imageData = reply->readAll();
                QPixmap pixmap;
                if (pixmap.loadFromData(imageData)) {
                    QPixmap scaledPixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                    m_avatarLabel->setPixmap(scaledPixmap);
                }
            }
            reply->deleteLater();
        });
    }
}

void UserInfoPage::showError(const QString &message)
{
    QMessageBox::warning(this, "错误", message);
}

QWidget* UserInfoPage::createInfoItem(const QString &label, const QString &value)
{
    QWidget *widget = new QWidget();
    widget->setStyleSheet(
        "QWidget { "
        "    background-color: #f8f9fa; "
        "    border-radius: 10px; "
        "    padding: 15px; "
        "}"
    );
    
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(15, 10, 15, 10);
    layout->setSpacing(15);
    
    // 标签
    QLabel *labelWidget = new QLabel(label);
    labelWidget->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "    color: #6c757d; "
        "    min-width: 80px; "
        "}"
    );
    
    // 值
    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("valueLabel");
    valueLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 15px; "
        "    color: #2c3e50; "
        "    font-weight: 500; "
        "}"
    );
    valueLabel->setWordWrap(true);
    
    layout->addWidget(labelWidget);
    layout->addWidget(valueLabel, 1);
    
    return widget;
}