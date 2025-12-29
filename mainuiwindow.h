#ifndef MAINUIWINDOW_H
#define MAINUIWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStatusBar>
#include <QGridLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include "echarts/echartstab.h"
#include "echarts/logstatstab.h"
#include "loginpage.h"
#include "changepasswordpage.h"

class MainUIWindow : public QWidget
{
    Q_OBJECT

public:
    MainUIWindow(QWidget *parent = nullptr);

private slots:
    void onMainMenuClicked(QListWidgetItem *item);
    void onSubMenuClicked(QListWidgetItem *item);
    void onLoginSuccess(const QString &token);
    void onLogoutClicked();
    void updateUserInfo();

private:
    void setupUI(QWidget *parent = nullptr);
    void setupMainMenu();
    void setupSubMenu(const QString &mainMenu);
    void setupContent();
    QPixmap createCircularPixmap(const QPixmap &pixmap, int size);

    QGridLayout *mainLayout;
    QLabel *titleLabel;
    QLabel *avatarLabel;
    QLabel *usernameLabel;
    QPushButton *logoutButton;
    QListWidget *mainMenuList;
    QListWidget *subMenuList;
    QStackedWidget *contentStack;
    QStatusBar *statusBar;
    LoginPage *loginPage;
    QStackedWidget *mainStack;
    
    // 状态栏元素
    QLabel *statusIndicator;
    QLabel *statusText;
    QLabel *networkStatus;
    QLabel *timeLabel;
    QLabel *statusMessage;
};

#endif // MAINUIWINDOW_H
