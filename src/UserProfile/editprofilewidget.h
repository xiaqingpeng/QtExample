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
    QLineEdit *m_nicknameEdit;
    QLineEdit *m_douBaoIdEdit;
    QLabel *m_avatarLabel;
    
    // 辅助函数：创建圆形头像
    QPixmap roundAvatar(const QPixmap &pixmap, int size);
};

#endif // EDITPROFILEWIDGET_H
