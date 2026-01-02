#ifndef CHANGEPASSWORDPAGE_H
#define CHANGEPASSWORDPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "networkmanager.h"

class ChangePasswordPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChangePasswordPage(QWidget *parent = nullptr);

signals:
    void passwordChanged();

private slots:
    void onChangePasswordClicked();
    void onThemeChanged(); // 主题变化槽函数

private:
    void applyTheme(); // 应用主题方法
    QLineEdit *m_oldPasswordEdit;
    QLineEdit *m_newPasswordEdit;
    QLineEdit *m_confirmPasswordEdit;
    QPushButton *m_changePasswordBtn;
    NetworkManager *m_networkManager;
};

#endif // CHANGEPASSWORDPAGE_H