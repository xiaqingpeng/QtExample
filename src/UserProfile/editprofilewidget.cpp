#include "editprofilewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QRegularExpression>

EditProfileWidget::EditProfileWidget(QWidget *parent) : QWidget(parent)
{
    // 顶部栏：取消 + 标题 + 完成
    QHBoxLayout *topLayout = new QHBoxLayout;
    QPushButton *cancelBtn = new QPushButton("取消");
    QLabel *titleLabel = new QLabel("个人资料");
    titleLabel->setAlignment(Qt::AlignCenter);
    QPushButton *saveBtn = new QPushButton("完成");
    saveBtn->setObjectName("saveBtn");

    topLayout->addWidget(cancelBtn);
    topLayout->addWidget(titleLabel, 1); // 占满中间空间
    topLayout->addWidget(saveBtn);

    // 头像区域：圆形头像 + 相机图标
    QWidget *avatarContainer = new QWidget;
    avatarContainer->setFixedSize(140, 160); // 稍大一些，便于摆放圆形头像与相机按钮
    
    // 头像标签 - 使用绝对定位
    m_avatarLabel = new QLabel(avatarContainer);
    const int avatarSize = 110;
    QPixmap avatarPixmap(":/images/avatar.jpg");
    if (avatarPixmap.isNull()) {
        // Create a simple placeholder avatar
        avatarPixmap = QPixmap(avatarSize, avatarSize);
        avatarPixmap.fill(Qt::transparent);
        QPainter painter(&avatarPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(QBrush(QColor(200, 200, 200)));
        painter.setPen(QPen(QColor(150, 150, 150), 2));
        painter.drawEllipse(0, 0, avatarSize, avatarSize);
        painter.setPen(QPen(QColor(120, 120, 120), 2));
        painter.drawText(avatarPixmap.rect(), Qt::AlignCenter, "头像");
    }
    QPixmap avatarPix = roundAvatar(avatarPixmap, avatarSize);
    m_avatarLabel->setPixmap(avatarPix);
    m_avatarLabel->setFixedSize(avatarSize, avatarSize);
    m_avatarLabel->setStyleSheet("background: transparent; border: none;");
    // 不要使用 setScaledContents，因为 pixmap 已经是正确尺寸的圆形
    m_avatarLabel->setScaledContents(false);
    // 头像居中：容器宽度140，头像宽度avatarSize
    const int avatarX = (avatarContainer->width() - avatarSize) / 2;
    const int avatarY = 8;
    m_avatarLabel->move(avatarX, avatarY);

    // 相机按钮 - 使用绝对定位
    QPushButton *cameraBtn = new QPushButton(avatarContainer);
    QIcon cameraIcon(":/icons/camera.png");
    if (cameraIcon.isNull()) {
        // Create a simple camera placeholder
        QPixmap cameraPixmap(30, 30);
        cameraPixmap.fill(Qt::transparent);
        QPainter painter(&cameraPixmap);
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(5, 8, 20, 14);
        painter.drawRect(10, 5, 10, 6);
        cameraIcon = QIcon(cameraPixmap);
    }
    cameraBtn->setIcon(cameraIcon);
    cameraBtn->setIconSize(QSize(18, 18));
    const int camBtnSize = 32;
    cameraBtn->setFixedSize(camBtnSize, camBtnSize);
    cameraBtn->setStyleSheet("QPushButton { border-radius:16px; background-color:#007aff; border: 2px solid white; }");
    
    // 相机图标位置计算：在圆形头像正下方居中
    // 头像中心X坐标 = avatarX + avatarSize/2
    // 相机按钮中心X坐标 = 头像中心X坐标
    // 相机按钮X坐标 = 头像中心X - 相机按钮宽度/2
    const int avatarCenterX = avatarX + avatarSize / 2;
    const int camX = avatarCenterX - camBtnSize / 2;
    
    // 相机按钮Y坐标：让相机按钮的中心在头像底部边缘
    // 头像底部Y坐标 = avatarY + avatarSize
    // 相机按钮中心Y坐标 = 头像底部Y坐标
    // 相机按钮Y坐标 = 头像底部Y坐标 - 相机按钮高度/2
    const int avatarBottomY = avatarY + avatarSize;
    const int camY = avatarBottomY - camBtnSize / 2;
    
    cameraBtn->move(camX, camY);
    cameraBtn->raise(); // 确保在头像之上

    // 使用QHBoxLayout来居中整个头像容器
    QHBoxLayout *avatarLayout = new QHBoxLayout;
    avatarLayout->addStretch();
    avatarLayout->addWidget(avatarContainer);
    avatarLayout->addStretch();

    // 昵称输入框
    QWidget *nicknameWidget = new QWidget;
    QVBoxLayout *nicknameLayout = new QVBoxLayout(nicknameWidget);
    QLabel *nicknameLabel = new QLabel("昵称");
    m_nicknameEdit = new QLineEdit("tony");
    m_nicknameEdit->setFixedHeight(40);
    nicknameLayout->addWidget(nicknameLabel);
    nicknameLayout->addWidget(m_nicknameEdit);

    // 豆包号输入框（正则校验：字母、数字、下划线，4-16位）
    QWidget *douBaoIdWidget = new QWidget;
    QVBoxLayout *douBaoIdLayout = new QVBoxLayout(douBaoIdWidget);
    QLabel *douBaoIdLabel = new QLabel("用户ID");
    m_douBaoIdEdit = new QLineEdit("user_4947505154");
    m_douBaoIdEdit->setFixedHeight(40);
    QRegularExpression regExp("^[a-zA-Z0-9_]{4,16}$");
    m_douBaoIdEdit->setValidator(new QRegularExpressionValidator(regExp, this));
    // QLabel *tipsLabel = new QLabel("豆包号只能使用字母、数字以及下划线，长度为4-16个字符");
    // tipsLabel->setStyleSheet("color:#999999; font-size:12px;");
    douBaoIdLayout->addWidget(douBaoIdLabel);
    douBaoIdLayout->addWidget(m_douBaoIdEdit);
    // douBaoIdLayout->addWidget(tipsLabel);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(avatarLayout); // 使用avatarLayout而不是avatarWidget
    mainLayout->addWidget(nicknameWidget);
    mainLayout->addWidget(douBaoIdWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(30);

    // 信号槽连接
    connect(cancelBtn, &QPushButton::clicked, this, &EditProfileWidget::cancelEdit);
    connect(saveBtn, &QPushButton::clicked, this, &EditProfileWidget::onSaveClicked);
    connect(cameraBtn, &QPushButton::clicked, [=]() {
        QString filePath = QFileDialog::getOpenFileName(this, "选择头像", "", "图片文件 (*.png *.jpg *.jpeg)");
        if (!filePath.isEmpty()) {
            QPixmap newAvatar(filePath);
            if (!newAvatar.isNull()) {
                // 使用相同的 avatarSize 保持一致性
                m_avatarLabel->setPixmap(roundAvatar(newAvatar, avatarSize));
            }
        }
    });
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
    int x = (size - scaledPixmap.width()) / 2;
    int y = (size - scaledPixmap.height()) / 2;
    
    // 绘制图像
    painter.drawPixmap(x, y, scaledPixmap);
    
    // 添加圆形边框
    painter.setClipping(false);
    QPen borderPen(QColor(220, 220, 220), 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(0, 0, size-1, size-1);
    
    return result;
}
