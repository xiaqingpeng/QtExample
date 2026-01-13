#include "editprofilewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QRegularExpression>
#include <QBrush>
#include <QPen>

EditProfileWidget::EditProfileWidget(QWidget *parent) : QWidget(parent)
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(MAIN_MARGIN, MAIN_MARGIN, MAIN_MARGIN, MAIN_MARGIN);
    mainLayout->setSpacing(MAIN_SPACING);
    
    // 添加各个部分
    mainLayout->addLayout(createTopBar());
    mainLayout->addLayout(createAvatarSection());
    
    // 创建输入区域
    QWidget *nicknameWidget = createInputSection("昵称", m_nicknameEdit, "tony");
    QWidget *douBaoIdWidget = createInputSection("用户ID", m_douBaoIdEdit, "user_4947505154", 
                                                   QString("^[a-zA-Z0-9_]{%1,%2}$")
                                                   .arg(DOUBAO_ID_MIN_LENGTH)
                                                   .arg(DOUBAO_ID_MAX_LENGTH));
    
    mainLayout->addWidget(nicknameWidget);
    mainLayout->addWidget(douBaoIdWidget);
}

QHBoxLayout* EditProfileWidget::createTopBar()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    
    QPushButton *cancelBtn = new QPushButton("取消");
    QLabel *titleLabel = new QLabel("个人资料");
    titleLabel->setAlignment(Qt::AlignCenter);
    QPushButton *saveBtn = new QPushButton("完成");
    saveBtn->setObjectName("saveBtn");

    topLayout->addWidget(cancelBtn);
    topLayout->addWidget(titleLabel, 1);
    topLayout->addWidget(saveBtn);

    // 信号槽连接
    connect(cancelBtn, &QPushButton::clicked, this, &EditProfileWidget::cancelEdit);
    connect(saveBtn, &QPushButton::clicked, this, &EditProfileWidget::onSaveClicked);
    
    return topLayout;
}

QHBoxLayout* EditProfileWidget::createAvatarSection()
{
    // 创建头像容器
    QWidget *avatarContainer = new QWidget;
    avatarContainer->setFixedSize(AVATAR_CONTAINER_WIDTH, AVATAR_CONTAINER_HEIGHT);
    
    // 创建头像标签
    m_avatarLabel = new QLabel(avatarContainer);
    QPixmap avatarPixmap(":/images/avatar.jpg");
    if (avatarPixmap.isNull()) {
        avatarPixmap = createPlaceholderAvatar(AVATAR_SIZE);
    }
    setupAvatarLabel(m_avatarLabel, roundAvatar(avatarPixmap, AVATAR_SIZE), AVATAR_SIZE);
    
    // 计算头像位置（必须在容器设置大小之后）
    const int avatarX = (AVATAR_CONTAINER_WIDTH - AVATAR_SIZE) / 2;
    const int avatarY = AVATAR_TOP_MARGIN;
    m_avatarLabel->move(avatarX, avatarY);

    // 创建相机按钮
    QPushButton *cameraBtn = new QPushButton(avatarContainer);
    QIcon cameraIcon(":/icons/camera.png");
    if (cameraIcon.isNull()) {
        cameraIcon = createCameraIcon();
    }
    cameraBtn->setIcon(cameraIcon);
    cameraBtn->setIconSize(QSize(CAMERA_ICON_SIZE, CAMERA_ICON_SIZE));
    cameraBtn->setFixedSize(CAMERA_BUTTON_SIZE, CAMERA_BUTTON_SIZE);
    cameraBtn->setStyleSheet("QPushButton { border-radius:16px; background-color:#007aff; border: 2px solid white; }");
    
    setupCameraButton(cameraBtn, avatarContainer, avatarX, avatarY, AVATAR_SIZE);
    cameraBtn->raise();
    
    // 连接相机按钮信号
    connect(cameraBtn, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "选择头像", "", 
                                                         "图片文件 (*.png *.jpg *.jpeg)");
        if (!filePath.isEmpty()) {
            QPixmap newAvatar(filePath);
            if (!newAvatar.isNull()) {
                m_avatarLabel->setPixmap(roundAvatar(newAvatar, AVATAR_SIZE));
            }
        }
    });

    // 居中头像容器
    QHBoxLayout *avatarLayout = new QHBoxLayout;
    avatarLayout->addStretch();
    avatarLayout->addWidget(avatarContainer);
    avatarLayout->addStretch();

    return avatarLayout;
}

QWidget* EditProfileWidget::createInputSection(const QString &labelText, QLineEdit *&lineEdit, 
                                                const QString &defaultValue, 
                                                const QString &validatorPattern)
{
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
    QLabel *label = new QLabel(labelText);
    lineEdit = new QLineEdit(defaultValue);
    lineEdit->setFixedHeight(INPUT_HEIGHT);
    
    if (!validatorPattern.isEmpty()) {
        QRegularExpression regExp(validatorPattern);
        lineEdit->setValidator(new QRegularExpressionValidator(regExp, this));
    }
    
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    
    return widget;
}

QPixmap EditProfileWidget::createPlaceholderAvatar(int size)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // 绘制背景圆
    painter.setBrush(QBrush(QColor(200, 200, 200)));
    painter.setPen(QPen(QColor(150, 150, 150), 2));
    painter.drawEllipse(0, 0, size, size);

    // 绘制文字
    painter.setPen(QPen(QColor(120, 120, 120), 2));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, "头像");
    
    return pixmap;
}

QIcon EditProfileWidget::createCameraIcon()
{
    const int iconSize = 30;
    QPixmap cameraPixmap(iconSize, iconSize);
    cameraPixmap.fill(Qt::transparent);
    
    QPainter painter(&cameraPixmap);
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(QBrush(Qt::white));
    
    // 绘制相机图标
    painter.drawRect(5, 8, 20, 14);
    painter.drawRect(10, 5, 10, 6);
    
    return QIcon(cameraPixmap);
}

void EditProfileWidget::setupAvatarLabel(QLabel *label, const QPixmap &pixmap, int size)
{
    label->setPixmap(pixmap);
    label->setFixedSize(size, size);
    label->setStyleSheet("background: transparent; border: none;");
    label->setScaledContents(false);
}

void EditProfileWidget::setupCameraButton(QPushButton *button, QWidget */*parent*/, 
                                          int avatarX, int avatarY, int avatarSize)
{
    // 计算相机按钮位置：在头像底部边缘居中
    const int avatarCenterX = avatarX + avatarSize / 2;
    const int avatarBottomY = avatarY + avatarSize;
    
    const int camX = avatarCenterX - CAMERA_BUTTON_SIZE / 2;
    const int camY = avatarBottomY - CAMERA_BUTTON_SIZE / 2;
    
    button->move(camX, camY);
}

QString EditProfileWidget::getNickname() const
{
    return m_nicknameEdit->text();
}

QString EditProfileWidget::getDouBaoId() const
{
    return m_douBaoIdEdit->text();
}

void EditProfileWidget::onSaveClicked()
{
    if (m_douBaoIdEdit->hasAcceptableInput()) {
        emit saveProfile(getNickname(), getDouBaoId());
    }
}

QPixmap EditProfileWidget::roundAvatar(const QPixmap &pixmap, int size)
{
    if (pixmap.isNull() || size <= 0) {
        return QPixmap();
    }
    
    // 创建目标圆形图像
    QPixmap result(size, size);
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    // 创建圆形裁剪区域
    QPainterPath clipPath;
    clipPath.addEllipse(0, 0, size, size);
    painter.setClipPath(clipPath);
    
    // 将原图像缩放到正方形，使用KeepAspectRatioByExpanding确保填满圆形
    QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    // 计算居中位置
    const int x = (size - scaledPixmap.width()) / 2;
    const int y = (size - scaledPixmap.height()) / 2;
    
    // 绘制图像
    painter.drawPixmap(x, y, scaledPixmap);
    
    // 添加圆形边框
    painter.setClipping(false);
    const QPen borderPen(QColor(220, 220, 220), 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(0, 0, size - 1, size - 1);
    
    return result;
}
