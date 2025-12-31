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
#include <QFileDialog>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFile>
#include <QJsonParseError>
#include <QPainter>
#include <QBrush>
#include <QRadialGradient>
#include <QGraphicsDropShadowEffect>
#include <QElapsedTimer>

UserInfoPage::UserInfoPage(QWidget *parent)
    : QWidget(parent)
    , m_networkManager(new NetworkManager(this))
{
    // 初始化Analytics SDK
    Analytics::Config config;
    config.serverUrl = "http://120.48.95.51:7001/api/analytics/events";
    config.appId = "qt-example-app";
    config.enableDebug = true;
    config.batchSize = 1;  // 降低批量大小，每个事件立即发送
    config.flushInterval = 5;  // 降低刷新间隔，5秒定时发送
    config.enablePersistence = true;
    Analytics::SDK::instance()->initialize(config);
    
    // 设置用户ID（从设置中获取）
    QSettings settings("YourCompany", "QtApp");
    QString userId = settings.value("user/id", "").toString();
    if (!userId.isEmpty()) {
        Analytics::SDK::instance()->setUserId(userId);
    }
    
    setupUI();
    loadUserInfo();
    
    // 追踪页面浏览事件
    Analytics::SDK::instance()->trackView("user_info_page", {
        {"page_title", "个人中心"},
        {"user_id", userId}
    });
}

UserInfoPage::~UserInfoPage()
{
    // 确保所有埋点事件都被上报
    Analytics::SDK::instance()->flush();
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
    
    // 头像容器（带渐变边框和阴影）
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setFixedSize(160, 160);
    avatarContainer->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    border-radius: 80px; "
        "}"
    );
    
    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(5, 5, 5, 5);
    
    m_avatarLabel = new QLabel();
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setFixedSize(150, 150);
    // QQ风格头像：白色边框 + 阴影效果
    m_avatarLabel->setStyleSheet(
        "QLabel { "
        "    background-color: white; "
        "    border-radius: 75px; "
        "    border: 4px solid white; "
        "}"
    );
    // 添加阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 4);
    m_avatarLabel->setGraphicsEffect(shadowEffect);
    
    // QQ风格在线状态指示器
    QLabel *onlineIndicator = new QLabel();
    onlineIndicator->setFixedSize(24, 24);
    onlineIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #12B7F5; "
        "    border-radius: 12px; "
        "    border: 3px solid white; "
        "}"
    );
    // 添加阴影效果
    QGraphicsDropShadowEffect *indicatorShadow = new QGraphicsDropShadowEffect(this);
    indicatorShadow->setBlurRadius(8);
    indicatorShadow->setColor(QColor(0, 0, 0, 60));
    indicatorShadow->setOffset(0, 2);
    onlineIndicator->setGraphicsEffect(indicatorShadow);
    
    // 使用绝对定位将状态指示器放在头像右下角
    onlineIndicator->setParent(avatarContainer);
    onlineIndicator->move(132, 132);
    
    // 设置默认头像（使用更美观的渐变和用户图标）
    QPixmap defaultAvatar(142, 142);
    defaultAvatar.fill(Qt::transparent);
    QPainter painter(&defaultAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 创建圆形渐变背景
    QRadialGradient gradient(71, 71, 71);
    gradient.setColorAt(0, QColor("#f8f9fa"));
    gradient.setColorAt(1, QColor("#e9ecef"));
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 142, 142);
    
    // 绘制用户图标
    painter.setBrush(QBrush(QColor("#adb5bd")));
    painter.drawEllipse(42, 36, 58, 58); // 头部
    painter.drawEllipse(20, 101, 102, 58); // 身体
    
    m_avatarLabel->setPixmap(defaultAvatar);
    
    avatarLayout->addWidget(m_avatarLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(avatarContainer, 0, Qt::AlignCenter);
    
    // 上传头像按钮
    m_uploadAvatarButton = new QPushButton("更换头像");
    m_uploadAvatarButton->setStyleSheet(
        "QPushButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    color: white; "
        "    border: none; "
        "    padding: 10px 30px; "
        "    border-radius: 20px; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #5568d3, stop:1 #653a8a); "
        "}"
        "QPushButton:pressed { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #4457c2, stop:1 #542978); "
        "}"
    );
    connect(m_uploadAvatarButton, &QPushButton::clicked, this, &UserInfoPage::onUploadAvatarClicked);
    cardLayout->addWidget(m_uploadAvatarButton, 0, Qt::AlignCenter);
    
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
    
    // 用户状态
    QWidget *statusWidget = createInfoItem("账户状态", "正常");
    m_statusLabel = statusWidget->findChild<QLabel*>("valueLabel");
    m_statusLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 15px; "
        "    color: #28a745; "
        "    font-weight: bold; "
        "}"
    );
    infoItemsLayout->addWidget(statusWidget);
    
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
                    // 创建圆形头像（142px大小以匹配头像标签尺寸）
                    QPixmap circularAvatar = createCircularPixmap(pixmap, 142);
                    m_avatarLabel->setPixmap(circularAvatar);
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

QPixmap UserInfoPage::createCircularPixmap(const QPixmap &pixmap, int size)
{
    // 创建指定大小的圆形图片
    QPixmap circularPixmap(size, size);
    circularPixmap.fill(Qt::transparent);
    
    // 缩放原始图片以适应圆形
    QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    // 创建圆形遮罩
    QBitmap mask(size, size);
    mask.fill(Qt::transparent);
    
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);
    painter.end();
    
    // 应用遮罩
    scaledPixmap.setMask(mask);
    
    // 将遮罩后的图片绘制到透明背景上
    QPainter finalPainter(&circularPixmap);
    finalPainter.setRenderHint(QPainter::Antialiasing);
    finalPainter.drawPixmap(0, 0, scaledPixmap);
    finalPainter.end();
    
    return circularPixmap;
}

QWidget* UserInfoPage::createInfoItem(const QString &label, const QString &value)
{
    QWidget *widget = new QWidget();
    widget->setStyleSheet(
        "QWidget { "
        "    background-color: #ffffff; "
        "    border-radius: 12px; "
        "    border: 1px solid #e9ecef; "
        "}"
    );
    
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(20, 15, 20, 15);
    layout->setSpacing(15);
    
    // 图标容器
    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(40, 40);
    iconLabel->setStyleSheet(
        "QLabel { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    border-radius: 20px; "
        "}"
    );
    
    // 根据不同的标签显示不同的图标
    QString iconText = "👤";
    if (label == "邮箱") iconText = "📧";
    else if (label == "用户ID") iconText = "🆔";
    else if (label == "注册时间") iconText = "📅";
    else if (label == "账户状态") iconText = "✓";
    
    QLabel *iconTextWidget = new QLabel(iconText);
    iconTextWidget->setAlignment(Qt::AlignCenter);
    iconTextWidget->setStyleSheet(
        "QLabel { "
        "    font-size: 18px; "
        "}"
    );
    
    QVBoxLayout *iconLayout = new QVBoxLayout(iconLabel);
    iconLayout->setContentsMargins(0, 0, 0, 0);
    iconLayout->addWidget(iconTextWidget, 0, Qt::AlignCenter);
    
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
    
    layout->addWidget(iconLabel);
    layout->addWidget(labelWidget);
    layout->addWidget(valueLabel, 1);
    
    return widget;
}

void UserInfoPage::onUploadAvatarClicked()
{
    // 追踪点击事件
    Analytics::SDK::instance()->trackClick("upload_avatar_button", {
        {"page", "user_info"},
        {"button_text", "更换头像"}
    });
    
    // 打开文件选择对话框
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择头像图片",
        "",
        "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // 验证文件大小（限制为5MB）
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        showError("无法打开文件");
        return;
    }
    
    qint64 fileSize = file.size();
    file.close();
    
    if (fileSize > 5 * 1024 * 1024) {
        showError("文件大小不能超过5MB");
        return;
    }
    
    // 上传头像
    uploadAvatar(filePath);
}

void UserInfoPage::uploadAvatar(const QString &filePath)
{
    // 开始性能计时
    QElapsedTimer timer;
    timer.start();
    
    // 使用NetworkManager的uploadFile方法上传头像
    m_networkManager->uploadFile(
        "http://120.48.95.51:7001/api/upload/image",
        filePath,
        "file",
        [this, timer](const QJsonObject &response) {
            // 记录上传性能
            qint64 uploadTime = timer.elapsed();
            Analytics::SDK::instance()->trackPerformance("avatar_upload_time", uploadTime, {
                {"page", "user_info"},
                {"file_type", "image"},
                {"status", "success"}
            });
            
            // 成功回调
            // 检查响应结构
            if (!response.contains("data") || !response["data"].isObject()) {
                showError("服务器返回数据格式错误");
                return;
            }
            
            QJsonObject dataObj = response["data"].toObject();
            
            if (!dataObj.contains("url")) {
                showError("服务器未返回图片URL");
                return;
            }
            
            QString imageUrl = dataObj["url"].toString();
            
            // 移除URL中的引号（如果存在）
            imageUrl = imageUrl.remove('"').trimmed();
            
            // 更新设置中的头像URL
            QSettings settings("YourCompany", "QtApp");
            settings.setValue("user/avatar", imageUrl);
            settings.sync();
            
            // 重新加载用户信息以显示新头像
            loadUserInfo();
            
            // 发射头像更新信号，通知主窗口更新导航栏头像
            emit avatarUpdated();
            
            QMessageBox::information(this, "成功", "头像上传成功！");
        },
        [this, timer](const QString &errorMsg) {
            // 记录上传失败性能
            qint64 uploadTime = timer.elapsed();
            Analytics::SDK::instance()->trackPerformance("avatar_upload_time", uploadTime, {
                {"page", "user_info"},
                {"file_type", "image"},
                {"status", "failed"},
                {"error_message", errorMsg}
            });
            
            // 错误回调
            showError("上传失败: " + errorMsg);
        }
    );
}

void UserInfoPage::onAvatarUploadFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = "上传失败: " + reply->errorString();
        qDebug() << "Avatar upload error:" << errorMsg;
        
        // 读取服务器返回的详细错误信息
        QByteArray errorData = reply->readAll();
        if (!errorData.isEmpty()) {
            qDebug() << "Server error response:" << QString::fromUtf8(errorData);
            errorMsg += "\n服务器详情: " + QString::fromUtf8(errorData);
        }
        
        showError(errorMsg);
        reply->deleteLater();
        return;
    }
    
    // 读取响应数据
    QByteArray responseData = reply->readAll();
    QString responseString = QString::fromUtf8(responseData);
    
    qDebug() << "Avatar upload response:" << responseString;
    
    // 解析JSON响应
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        showError("服务器响应格式错误");
        reply->deleteLater();
        return;
    }
    
    QJsonObject jsonObj = jsonDoc.object();
    
    if (!jsonObj.contains("url")) {
        showError("服务器未返回图片URL");
        reply->deleteLater();
        return;
    }
    
    QString imageUrl = jsonObj["url"].toString();
    
    // 移除URL中的引号（如果存在）
    imageUrl = imageUrl.remove('"').trimmed();
    
    qDebug() << "Uploaded avatar URL:" << imageUrl;
    
    // 更新设置中的头像URL
    QSettings settings("YourCompany", "QtApp");
    settings.setValue("user/avatar", imageUrl);
    settings.sync();
    
    // 重新加载用户信息以显示新头像
    loadUserInfo();
    
    // 发射头像更新信号，通知主窗口更新导航栏头像
    emit avatarUpdated();
    
    QMessageBox::information(this, "成功", "头像上传成功！");
    
    reply->deleteLater();
}