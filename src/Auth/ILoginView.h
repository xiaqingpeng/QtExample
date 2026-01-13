#ifndef ILOGINVIEW_H
#define ILOGINVIEW_H

#include <QString>

// 登录视图接口：用于将 LoginPage 与控制器解耦
class ILoginView
{
public:
    virtual ~ILoginView() = default;

    // 读取当前输入
    virtual QString email() const = 0;
    virtual QString password() const = 0;
    virtual bool rememberPasswordChecked() const = 0;

    // 设置输入内容
    virtual void setEmail(const QString &value) = 0;
    virtual void setPassword(const QString &value) = 0;

    // 状态展示
    virtual void showError(const QString &message) = 0;
    virtual void showSuccess(const QString &message) = 0;

    // 控制“忙碌”状态（例如登录中，按钮禁用等）
    virtual void setBusy(bool busy) = 0;
};

#endif // ILOGINVIEW_H


