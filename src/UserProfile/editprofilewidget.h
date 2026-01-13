#ifndef EDITPROFILEWIDGET_H
#define EDITPROFILEWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpressionValidator>

class EditProfileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditProfileWidget(QWidget *parent = nullptr);

    // 获取编辑后的昵称和豆包号
    QString getNickname() const;
    QString getDouBaoId() const;

private slots:
    void onSaveClicked(); // 保存按钮点击事件

signals:
    void saveProfile(const QString &nickname, const QString &douBaoId); // 保存信号
    void cancelEdit(); // 取消编辑信号

private:
    // UI组件
    QLineEdit *m_nicknameEdit;
    QLineEdit *m_douBaoIdEdit;
    QLabel *m_avatarLabel;
    
    // 布局创建方法
    QHBoxLayout* createTopBar();
    QHBoxLayout* createAvatarSection();
    QWidget* createInputSection(const QString &labelText, QLineEdit *&lineEdit, 
                                 const QString &defaultValue = QString(), 
                                 const QString &validatorPattern = QString());
    
    // 辅助函数
    QPixmap roundAvatar(const QPixmap &pixmap, int size);
    QPixmap createPlaceholderAvatar(int size);
    QIcon createCameraIcon();
    void setupAvatarLabel(QLabel *label, const QPixmap &pixmap, int size);
    void setupCameraButton(QPushButton *button, QWidget *parent, int avatarX, int avatarY, int avatarSize);
    
    // 常量
    static constexpr int AVATAR_SIZE = 110;
    static constexpr int AVATAR_CONTAINER_WIDTH = 140;
    static constexpr int AVATAR_CONTAINER_HEIGHT = 160;
    static constexpr int AVATAR_TOP_MARGIN = 8;
    static constexpr int CAMERA_BUTTON_SIZE = 32;
    static constexpr int CAMERA_ICON_SIZE = 18;
    static constexpr int INPUT_HEIGHT = 40;
    static constexpr int MAIN_MARGIN = 20;
    static constexpr int MAIN_SPACING = 30;
    static constexpr int DOUBAO_ID_MIN_LENGTH = 4;
    static constexpr int DOUBAO_ID_MAX_LENGTH = 16;
};

#endif // EDITPROFILEWIDGET_H
