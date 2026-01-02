#include "userinfopage.h"
#include "../styles/theme_manager.h"
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
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>
// #include <stdexcept>

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
    
    // 连接主题变化信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, 
            this, &UserInfoPage::onThemeChanged);
    
    // 应用当前主题
    QTimer::singleShot(0, this, &UserInfoPage::applyTheme);
    
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
    
    // 设置页面对象名
    this->setObjectName("userInfoPage");
    
    // 顶部装饰区域
    QWidget *headerWidget = new QWidget();
    headerWidget->setObjectName("topBar");
    headerWidget->setFixedHeight(80);
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    // 标题
    QLabel *titleLabel = new QLabel("个人中心");
    titleLabel->setObjectName("logoLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(titleLabel);
    
    // 副标题
    QLabel *subtitleLabel = new QLabel("查看和管理您的个人信息");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    // headerLayout->addWidget(subtitleLabel);
    
    headerLayout->addStretch();
    mainLayout->addWidget(headerWidget);
    
    // 内容区域容器
    QWidget *contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(24, 20, 24, 24);
    contentLayout->setSpacing(20);
    
    // 主信息卡片
    QWidget *mainInfoCard = new QWidget();
    mainInfoCard->setObjectName("userInfoCard");
    
    QVBoxLayout *mainCardLayout = new QVBoxLayout(mainInfoCard);
    mainCardLayout->setContentsMargins(40, 40, 40, 40);
    mainCardLayout->setSpacing(32);
    
    // 头像和基本信息区域
    QWidget *profileSection = new QWidget();
    profileSection->setObjectName("profileSection");
    QVBoxLayout *profileLayout = new QVBoxLayout(profileSection);
    profileLayout->setSpacing(20);
    
    // 头像容器
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setObjectName("avatarContainer");
    avatarContainer->setFixedSize(160, 160);
    
    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(4, 4, 4, 4);
    
    m_avatarLabel = new QLabel();
    m_avatarLabel->setObjectName("avatarLabel");
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setFixedSize(152, 152);
    
    // 在线状态指示器
    QLabel *onlineIndicator = new QLabel();
    onlineIndicator->setFixedSize(24, 24);
    onlineIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #10b981; "
        "    border-radius: 12px; "
        "    border: 4px solid white; "
        "}"
    );
    
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
    
    // 上传头像按钮
    m_uploadAvatarButton = new QPushButton("更换头像");
    m_uploadAvatarButton->setObjectName("uploadAvatarButton");
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
    // 首先尝试加载本地头像
    QString localAvatarPath = settings.value("user/avatar_local", "").toString();
    
    if (!localAvatarPath.isEmpty() && QFileInfo::exists(localAvatarPath)) {
        QPixmap localPixmap(localAvatarPath);
        if (!localPixmap.isNull()) {
            QPixmap circularAvatar = createCircularPixmap(localPixmap, 152);
            m_avatarLabel->setPixmap(circularAvatar);
            qDebug() << "Loaded local avatar from:" << localAvatarPath;
            return; // 使用本地头像，不需要网络加载
        }
    }
    
    // 如果没有本地头像，尝试从网络加载
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
                 //   qDebug() << "Loaded network avatar";
                }
            } else {
             //   qDebug() << "Failed to load network avatar:" << reply->errorString();
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
    if (pixmap.isNull()) {
        return QPixmap();
    }
    
    // 创建指定大小的圆形图片
    QPixmap result(size, size);
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 设置圆形裁剪区域
    QPainterPath clipPath;
    clipPath.addEllipse(0, 0, size, size);
    painter.setClipPath(clipPath);
    
    // 计算缩放，确保图片能够完全覆盖圆形区域
    qreal scaleX = static_cast<qreal>(size) / pixmap.width();
    qreal scaleY = static_cast<qreal>(size) / pixmap.height();
    qreal scale = qMax(scaleX, scaleY); // 使用较大的缩放比例确保完全覆盖
    
    int scaledWidth = static_cast<int>(pixmap.width() * scale);
    int scaledHeight = static_cast<int>(pixmap.height() * scale);
    
    // 缩放图片
    QPixmap scaledPixmap = pixmap.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 计算居中位置
    int x = (size - scaledWidth) / 2;
    int y = (size - scaledHeight) / 2;
    
    // 绘制图片，确保完全填充圆形
    painter.drawPixmap(x, y, scaledPixmap);
    painter.end();
    
    return result;
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
    
    // 添加微妙阴影 (暂时禁用以排查崩溃问题)
    // QGraphicsDropShadowEffect *itemShadow = new QGraphicsDropShadowEffect(this);
    // itemShadow->setBlurRadius(8);
    // itemShadow->setColor(QColor(0, 0, 0, 4));
    // itemShadow->setOffset(0, 2);
    // widget->setGraphicsEffect(itemShadow);
    
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
    
    // 创建简单的文本标识符，避免emoji导致的崩溃
    QString iconText = "";
    if (label == "邮箱") iconText = "Email";
    else if (label == "用户ID") iconText = "ID";
    else if (label == "注册时间") iconText = "Date";
    else if (label == "账户状态") iconText = "Status";
    else iconText = "Info";
    
    QLabel *iconTextWidget = new QLabel(iconText);
    iconTextWidget->setAlignment(Qt::AlignCenter);
    iconTextWidget->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #64748b; "
        "    font-weight: 500; "
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
  //  qDebug() << "Upload avatar button clicked!";
    
    try {
        // 检查文件系统访问权限
        QString testDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (testDir.isEmpty()) {
            testDir = QDir::homePath();
        }
        
        QDir dir(testDir);
        if (!dir.exists()) {
         //   qDebug() << "Pictures directory does not exist:" << testDir;
            testDir = QDir::homePath();
        }
        
      //  qDebug() << "Using directory for file dialog:" << testDir;
        
        // 追踪点击事件 (添加异常保护)
        try {
            Analytics::SDK::instance()->trackClick("upload_avatar_button", {
                {"page", "user_info"},
                {"button_text", "更换头像"}
            });
        } catch (...) {
         //   qDebug() << "Analytics tracking failed, continuing...";
        }
        
        // 尝试不同的文件对话框方式
        QString filePath;
        
        // 方法1: 使用非原生对话框
     //   qDebug() << "Trying non-native dialog...";
        filePath = QFileDialog::getOpenFileName(
            this,
            "选择头像图片",
            testDir,
            "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif);;所有文件 (*.*)",
            nullptr,
            QFileDialog::DontUseNativeDialog
        );
        
        // 如果第一种方法失败，尝试原生对话框
        if (filePath.isEmpty()) {
          //  qDebug() << "Non-native dialog failed, trying native dialog...";
            filePath = QFileDialog::getOpenFileName(
                this,
                "选择头像图片",
                testDir,
                "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif);;所有文件 (*.*)"
            );
        }
        
        // 如果还是失败，尝试无父窗口
        if (filePath.isEmpty()) {
          //  qDebug() << "Native dialog failed, trying without parent...";
            filePath = QFileDialog::getOpenFileName(
                nullptr,
                "选择头像图片",
                testDir,
                "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif);;所有文件 (*.*)"
            );
        }
        
     //   qDebug() << "Final selected file path:" << filePath;
        
        if (filePath.isEmpty()) {
         //   qDebug() << "No file selected after all attempts";
            // 显示详细的帮助信息
            QMessageBox::information(this, "文件选择失败", 
                "无法打开文件选择对话框。\n\n"
                "可能的原因：\n"
                "1. 应用程序缺少文件访问权限\n"
                "2. 系统安全设置阻止了文件访问\n"
                "3. macOS沙盒限制\n\n"
                "解决方案：\n"
                "1. 在系统偏好设置 > 安全性与隐私 > 隐私 > 文件和文件夹中，\n"
                "   为此应用程序添加访问权限\n"
                "2. 尝试从应用程序文件夹运行程序\n"
                "3. 重启应用程序后重试");
            return;
        }
        
        // 验证文件是否存在
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists()) {
            showError("文件不存在: " + filePath);
            return;
        }
        
        if (!fileInfo.isReadable()) {
            showError("文件无法读取，请检查文件权限");
            return;
        }
        
        // 验证文件大小（限制为5MB）
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            showError("无法打开文件: " + file.errorString());
            return;
        }
        
        qint64 fileSize = file.size();
        file.close();
        
     //   qDebug() << "File size:" << fileSize << "bytes";
        
        if (fileSize > 5 * 1024 * 1024) {
            showError("文件大小不能超过5MB");
            return;
        }
        
        if (fileSize == 0) {
            showError("文件为空，请选择有效的图片文件");
            return;
        }
        
        // 验证是否为有效的图片文件
        QPixmap testPixmap(filePath);
        if (testPixmap.isNull()) {
            showError("无效的图片文件，请选择正确的图片格式");
            return;
        }
        
     //   qDebug() << "Image loaded successfully, size:" << testPixmap.size();
        
        // 先本地预览头像
        QPixmap circularAvatar = createCircularPixmap(testPixmap, 152);
        if (!circularAvatar.isNull()) {
            m_avatarLabel->setPixmap(circularAvatar);
         //   qDebug() << "Avatar preview updated successfully";
            
            // 立即发射信号，避免定时器
         //   qDebug() << "Emitting avatarUpdated signal immediately";
            emit avatarUpdated();
        }
        
        // 上传头像
        uploadAvatar(filePath);
        
    } catch (const std::exception& e) {
      //  qDebug() << "Exception in onUploadAvatarClicked:" << e.what();
        showError("发生未知错误，请重试");
    } catch (...) {
     //   qDebug() << "Unknown exception in onUploadAvatarClicked";
        showError("发生未知错误，请重试");
    }
}

void UserInfoPage::uploadAvatar(const QString &filePath)
{
 //   qDebug() << "Starting avatar upload for file:" << filePath;
    
    // 先更新本地显示
    QPixmap localPixmap(filePath);
    if (!localPixmap.isNull()) {
        QPixmap circularAvatar = createCircularPixmap(localPixmap, 152);
        m_avatarLabel->setPixmap(circularAvatar);
        
        // 保存到本地设置（临时方案）
        QSettings settings("YourCompany", "QtApp");
        settings.setValue("user/avatar_local", filePath);
        settings.sync();
        
        // 发射头像更新信号
   //     qDebug() << "Emitting avatarUpdated signal for local avatar";
        emit avatarUpdated();
        
        QMessageBox::information(this, "成功", "头像已更新！\n注意：这是本地预览，实际上传功能需要网络连接。");
        return;
    }
    
    // 如果需要网络上传，保留原有逻辑
    if (!m_networkManager) {
        showError("网络管理器未初始化");
        return;
    }
    
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
                    [](const QJsonObject &response) {
                        Q_UNUSED(response);
                        // 用户资料更新成功
                    },
                    [](const QString &error) {
                        Q_UNUSED(error);
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
            [](const QJsonObject &response) {
                Q_UNUSED(response);
                // 用户资料更新成功
            },
            [](const QString &error) {
                Q_UNUSED(error);
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

void UserInfoPage::onThemeChanged()
{
    applyTheme();
}

void UserInfoPage::applyTheme()
{
    ThemeManager* themeManager = ThemeManager::instance();
    const auto& colors = themeManager->colors();
    
    // 应用主题到页面
    QString pageStyle = QString(
        "QWidget#userInfoPage { "
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
        "    padding: 32px 0 12px 0; "
        "    letter-spacing: 1px; "
        "} "
        "QLabel#subtitleLabel { "
        "    color: rgba(255,255,255,0.9); "
        "    font-family: %6; "
        "    font-size: %7px; "
        "    font-weight: 400; "
        "    padding: 0 0 24px 0; "
        "} "
        "QWidget#contentWidget { "
        "    background-color: transparent; "
        "} "
        "QWidget#userInfoCard { "
        "    background-color: %8; "
        "    border: 1px solid %9; "
        "    border-radius: %10px; "
        "} "
        "QWidget#profileSection { "
        "    background-color: transparent; "
        "} "
        "QWidget#avatarContainer { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 %11, stop:1 %12); "
        "    border-radius: 80px; "
        "} "
        "QLabel#avatarLabel { "
        "    border: 2px solid %13; "
        "    border-radius: 76px; "
        "    background-color: %14; "
        "} "
        "QPushButton#uploadAvatarButton { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 %15, stop:1 %16); "
        "    color: white; "
        "    border: none; "
        "    padding: 14px 36px; "
        "    border-radius: %17px; "
        "    font-family: %18; "
        "    font-size: %19px; "
        "    font-weight: 600; "
        "    letter-spacing: 0.3px; "
        "} "
        "QPushButton#uploadAvatarButton:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 %20, stop:1 %21); "
        "} "
        "QPushButton#uploadAvatarButton:pressed { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 %22, stop:1 %23); "
        "}"
    ).arg(colors.BACKGROUND)
     .arg(colors.PRIMARY)
     .arg(colors.PRIMARY_HOVER)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_XL)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(colors.CARD)
     .arg(colors.BORDER)
     .arg(ThemeManager::BorderRadius::LG)
     .arg(colors.PRIMARY)
     .arg(colors.PRIMARY_HOVER)
     .arg(colors.PRIMARY)
     .arg(colors.SURFACE)
     .arg(colors.PRIMARY)           // 按钮背景起始色
     .arg(colors.PRIMARY_HOVER)     // 按钮背景结束色
     .arg(ThemeManager::BorderRadius::MD)  // 按钮圆角
     .arg(ThemeManager::Typography::FONT_FAMILY)  // 按钮字体
     .arg(ThemeManager::Typography::FONT_SIZE_SM) // 按钮字体大小
     .arg(colors.PRIMARY_HOVER)     // 悬停背景起始色
     .arg(colors.PRIMARY_LIGHT)     // 悬停背景结束色
     .arg(colors.PRIMARY_HOVER)     // 按下背景起始色
     .arg(colors.PRIMARY_LIGHT);    // 按下背景结束色
    
    // 应用按钮样式
    QString buttonStyle = themeManager->getButtonStyle("primary");
    
    // 应用所有样式
    this->setStyleSheet(pageStyle + buttonStyle);
}