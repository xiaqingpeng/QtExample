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
#include <QPainterPath>

UserInfoPage::UserInfoPage(QWidget *parent)
    : QWidget(parent)
    , m_networkManager(new NetworkManager(this))
{
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
    // 不需要手动flush，SDK会自动管理事件上报
}

void UserInfoPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 设置页面现代简约背景
    this->setStyleSheet(
        "QWidget { "
        "    background: #f8fafc; "
        "}"
    );
    
    // 顶部装饰区域（现代简约设计）
    QWidget *headerWidget = new QWidget();
    headerWidget->setFixedHeight(160);
    headerWidget->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #3b82f6, stop:1 #1d4ed8); "
        "    border-bottom: 1px solid #e2e8f0; "
        "}"
    );
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    // 标题
    QLabel *titleLabel = new QLabel("个人中心");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 28px; "
        "    font-weight: 600; "
        "    color: white; "
        "    padding: 32px 0 12px 0; "
        "    letter-spacing: 1px; "
        "}"
    );
    headerLayout->addWidget(titleLabel);
    
    // 副标题
    QLabel *subtitleLabel = new QLabel("查看和管理您的个人信息");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    color: rgba(255,255,255,0.9); "
        "    padding: 0 0 24px 0; "
        "    font-weight: 400; "
        "}"
    );
    headerLayout->addWidget(subtitleLabel);
    
    headerLayout->addStretch();
    mainLayout->addWidget(headerWidget);
    
    // 内容区域容器
    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("background-color: transparent;");
    
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(24, 20, 24, 24);
    contentLayout->setSpacing(20);
    
    // 主信息卡片（现代简约设计）
    QWidget *mainInfoCard = new QWidget();
    mainInfoCard->setStyleSheet(
        "QWidget { "
        "    background-color: #ffffff; "
        "    border-radius: 20px; "
        "    border: 1px solid #f1f5f9; "
        "}"
    );
    
    // 添加主卡片阴影
    QGraphicsDropShadowEffect *mainCardShadow = new QGraphicsDropShadowEffect(this);
    mainCardShadow->setBlurRadius(24);
    mainCardShadow->setColor(QColor(0, 0, 0, 6));
    mainCardShadow->setOffset(0, 8);
    mainInfoCard->setGraphicsEffect(mainCardShadow);
    
    QVBoxLayout *mainCardLayout = new QVBoxLayout(mainInfoCard);
    mainCardLayout->setContentsMargins(40, 40, 40, 40);
    mainCardLayout->setSpacing(32);
    
    // 头像和基本信息区域
    QWidget *profileSection = new QWidget();
    profileSection->setStyleSheet("background-color: transparent;");
    QVBoxLayout *profileLayout = new QVBoxLayout(profileSection);
    profileLayout->setSpacing(20);
    
    // 头像容器（现代简约设计）
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setFixedSize(160, 160);
    avatarContainer->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #3b82f6, stop:1 #1d4ed8); "
        "    border-radius: 80px; "
        "}"
    );
    
    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(4, 4, 4, 4);
    
    m_avatarLabel = new QLabel();
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setFixedSize(152, 152);
    // 现代简约头像样式
    m_avatarLabel->setStyleSheet(
        "QLabel { "
        "    background-color: #f8fafc; "
        "    border-radius: 76px; "
        "    border: 3px solid #ffffff; "
        "}"
    );
    
    // 添加微妙阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(16);
    shadowEffect->setColor(QColor(0, 0, 0, 12));
    shadowEffect->setOffset(0, 4);
    m_avatarLabel->setGraphicsEffect(shadowEffect);
    
    // 在线状态指示器（现代设计）
    QLabel *onlineIndicator = new QLabel();
    onlineIndicator->setFixedSize(24, 24);
    onlineIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #10b981; "
        "    border-radius: 12px; "
        "    border: 4px solid white; "
        "}"
    );
    
    // 添加指示器阴影
    QGraphicsDropShadowEffect *indicatorShadow = new QGraphicsDropShadowEffect(this);
    indicatorShadow->setBlurRadius(8);
    indicatorShadow->setColor(QColor(0, 0, 0, 20));
    indicatorShadow->setOffset(0, 2);
    onlineIndicator->setGraphicsEffect(indicatorShadow);
    
    // 使用绝对定位将状态指示器放在头像右下角
    onlineIndicator->setParent(avatarContainer);
    onlineIndicator->move(132, 132);
    
    // 设置默认头像（现代简约风格）
    QPixmap defaultAvatar(146, 146);
    defaultAvatar.fill(Qt::transparent);
    QPainter painter(&defaultAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 创建现代简约的渐变背景
    QRadialGradient gradient(73, 73, 73);
    gradient.setColorAt(0, QColor("#f1f5f9"));
    gradient.setColorAt(1, QColor("#e2e8f0"));
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 146, 146);
    
    // 绘制现代用户图标
    painter.setBrush(QBrush(QColor("#94a3b8")));
    painter.drawEllipse(44, 38, 58, 58); // 头部
    painter.drawEllipse(22, 102, 102, 58); // 身体
    
    m_avatarLabel->setPixmap(defaultAvatar);
    
    avatarLayout->addWidget(m_avatarLabel, 0, Qt::AlignCenter);
    profileLayout->addWidget(avatarContainer, 0, Qt::AlignCenter);
    
    // 用户名（现代简约风格）
    m_usernameLabel = new QLabel("加载中...");
    m_usernameLabel->setAlignment(Qt::AlignCenter);
    m_usernameLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 28px; "
        "    font-weight: 700; "
        "    color: #1e293b; "
        "    padding: 8px; "
        "    letter-spacing: 0.5px; "
        "}"
    );
    profileLayout->addWidget(m_usernameLabel);
    
    // 上传头像按钮（现代简约设计）
    m_uploadAvatarButton = new QPushButton("更换头像");
    m_uploadAvatarButton->setStyleSheet(
        "QPushButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #3b82f6, stop:1 #1d4ed8); "
        "    color: white; "
        "    border: none; "
        "    padding: 14px 36px; "
        "    border-radius: 10px; "
        "    font-size: 14px; "
        "    font-weight: 600; "
        "    letter-spacing: 0.3px; "
        "}"
        "QPushButton:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #2563eb, stop:1 #1e40af); "
        "    transform: translateY(-2px); "
        "}"
        "QPushButton:pressed { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #1d4ed8, stop:1 #1e3a8a); "
        "    transform: translateY(0px); "
        "}"
    );
    connect(m_uploadAvatarButton, &QPushButton::clicked, this, &UserInfoPage::onUploadAvatarClicked);
    profileLayout->addWidget(m_uploadAvatarButton, 0, Qt::AlignCenter);
    
    mainCardLayout->addWidget(profileSection);
    
    // 装饰分隔线（现代简约设计）
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setFixedHeight(1);
    line->setStyleSheet(
        "QFrame { "
        "    background-color: #f1f5f9; "
        "    border: none; "
        "    margin: 0 20px; "
        "}"
    );
    mainCardLayout->addWidget(line);
    
    // 详细信息区域
    QWidget *detailsSection = new QWidget();
    detailsSection->setStyleSheet("background-color: transparent;");
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsSection);
    detailsLayout->setSpacing(16);
    
    // 信息项容器
    QWidget *infoItemsWidget = new QWidget();
    QGridLayout *infoItemsLayout = new QGridLayout(infoItemsWidget);
    infoItemsLayout->setSpacing(16);
    infoItemsLayout->setContentsMargins(0, 0, 0, 0);
    
    // 用户ID
    QWidget *idWidget = createInfoItem("用户ID", "加载中...");
    m_idLabel = idWidget->findChild<QLabel*>("valueLabel");
    infoItemsLayout->addWidget(idWidget, 0, 0);
    
    // 邮箱
    QWidget *emailWidget = createInfoItem("邮箱", "加载中...");
    m_emailLabel = emailWidget->findChild<QLabel*>("valueLabel");
    infoItemsLayout->addWidget(emailWidget, 0, 1);
    
    // 注册时间
    QWidget *timeWidget = createInfoItem("注册时间", "加载中...");
    m_createTimeLabel = timeWidget->findChild<QLabel*>("valueLabel");
    infoItemsLayout->addWidget(timeWidget, 1, 0);
    
    // 用户状态
    QWidget *statusWidget = createInfoItem("账户状态", "正常");
    m_statusLabel = statusWidget->findChild<QLabel*>("valueLabel");
    m_statusLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    color: #10b981; "
        "    font-weight: 600; "
        "}"
    );
    infoItemsLayout->addWidget(statusWidget, 1, 1);
    
    detailsLayout->addWidget(infoItemsWidget);
    mainCardLayout->addWidget(detailsSection);
    
    contentLayout->addWidget(mainInfoCard);
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
    
    // 从设置加载用户信息
    
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
                    // 创建圆形头像（152px大小以匹配头像标签尺寸）
                    QPixmap circularAvatar = createCircularPixmap(pixmap, 152);
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
    
    // 计算缩放比例，确保图片完全填充圆形区域
    qreal scale = qMax(static_cast<qreal>(size) / pixmap.width(), 
                       static_cast<qreal>(size) / pixmap.height());
    
    // 缩放图片
    QPixmap scaledPixmap = pixmap.scaled(
        pixmap.width() * scale, 
        pixmap.height() * scale, 
        Qt::KeepAspectRatio, 
        Qt::SmoothTransformation
    );
    
    // 计算居中位置
    int x = (scaledPixmap.width() - size) / 2;
    int y = (scaledPixmap.height() - size) / 2;
    
    // 裁剪到指定大小
    QPixmap croppedPixmap = scaledPixmap.copy(x, y, size, size);
    
    // 创建最终的圆形图片
    QPainter finalPainter(&circularPixmap);
    finalPainter.setRenderHint(QPainter::Antialiasing);
    finalPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    
    // 设置圆形裁剪路径
    QPainterPath clipPath;
    clipPath.addEllipse(0, 0, size, size);
    finalPainter.setClipPath(clipPath);
    
    // 绘制图片
    finalPainter.drawPixmap(0, 0, croppedPixmap);
    finalPainter.end();
    
    return circularPixmap;
}

QWidget* UserInfoPage::createInfoItem(const QString &label, const QString &value)
{
    QWidget *widget = new QWidget();
    widget->setFixedHeight(100);
    widget->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #ffffff, stop:1 #f8fafc); "
        "    border-radius: 12px; "
        "    border: 1px solid #f1f5f9; "
        "}"
    );
    
    // 添加微妙阴影
    QGraphicsDropShadowEffect *itemShadow = new QGraphicsDropShadowEffect(this);
    itemShadow->setBlurRadius(8);
    itemShadow->setColor(QColor(0, 0, 0, 4));
    itemShadow->setOffset(0, 2);
    widget->setGraphicsEffect(itemShadow);
    
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);
    
    // 图标和标签容器
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);
    
    // 图标容器（现代简约设计）
    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(32, 32);
    iconLabel->setStyleSheet(
        "QLabel { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #3b82f6, stop:1 #1d4ed8); "
        "    border-radius: 16px; "
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
        "    font-size: 14px; "
        "}"
    );
    
    QVBoxLayout *iconLayout = new QVBoxLayout(iconLabel);
    iconLayout->setContentsMargins(0, 0, 0, 0);
    iconLayout->addWidget(iconTextWidget, 0, Qt::AlignCenter);
    
    // 标签（现代简约风格）
    QLabel *labelWidget = new QLabel(label);
    labelWidget->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    font-weight: 600; "
        "    color: #64748b; "
        "    letter-spacing: 0.3px; "
        "}"
    );
    
    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(labelWidget);
    headerLayout->addStretch();
    
    // 值（现代简约风格）
    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("valueLabel");
    valueLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 15px; "
        "    color: #1e293b; "
        "    font-weight: 600; "
        "    letter-spacing: 0.2px; "
        "}"
    );
    valueLabel->setWordWrap(true);
    
    layout->addLayout(headerLayout);
    layout->addWidget(valueLabel);
    layout->addStretch();
    
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
            
            // 同步头像URL到后端用户画像
            QString userId = settings.value("user/id", "").toString();
            if (!userId.isEmpty()) {
                QJsonObject profileData;
                profileData["avatar"] = imageUrl;
                
                m_networkManager->updateUserProfile(userId, profileData,
                    [this](const QJsonObject &response) {
                        // 用户资料更新成功
                    },
                    [this](const QString &error) {
                        qWarning() << "Failed to update user profile:" << error;
                        // 不阻塞用户，因为头像已经上传成功
                    }
                );
            }
            
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
        // 头像上传错误
        
        // 读取服务器返回的详细错误信息
        QByteArray errorData = reply->readAll();
        if (!errorData.isEmpty()) {
            // 服务器错误响应
            errorMsg += "\n服务器详情: " + QString::fromUtf8(errorData);
        }
        
        showError(errorMsg);
        reply->deleteLater();
        return;
    }
    
    // 读取响应数据
    QByteArray responseData = reply->readAll();
    QString responseString = QString::fromUtf8(responseData);
    
    // 头像上传响应
    
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
    
    // 已上传头像URL
    
    // 更新设置中的头像URL
    QSettings settings("YourCompany", "QtApp");
    settings.setValue("user/avatar", imageUrl);
    settings.sync();
    
    // 同步头像URL到后端用户画像
    QString userId = settings.value("user/id", "").toString();
    if (!userId.isEmpty()) {
        QJsonObject profileData;
        profileData["avatar"] = imageUrl;
        
        m_networkManager->updateUserProfile(userId, profileData,
            [this](const QJsonObject &response) {
                // 用户资料更新成功
            },
            [this](const QString &error) {
                qWarning() << "Failed to update user profile:" << error;
                // 不阻塞用户，因为头像已经上传成功
            }
        );
    }
    
    // 重新加载用户信息以显示新头像
    loadUserInfo();
    
    // 发射头像更新信号，通知主窗口更新导航栏头像
    emit avatarUpdated();
    
    QMessageBox::information(this, "成功", "头像上传成功！");
    
    reply->deleteLater();
}