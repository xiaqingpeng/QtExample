#ifndef MAINUIWINDOW_H
#define MAINUIWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStatusBar>
#include <QGridLayout>
#include "echarts/echartstab.h"
#include "loginpage.h"

class MainUIWindow : public QWidget
{
    Q_OBJECT

public:
    MainUIWindow(QWidget *parent = nullptr);

private slots:
    void onMainMenuClicked(QListWidgetItem *item);
    void onSubMenuClicked(QListWidgetItem *item);
    void onLoginSuccess(const QString &token);

private:
    void setupUI(QWidget *parent = nullptr);
    void setupMainMenu();
    void setupSubMenu(const QString &mainMenu);
    void setupContent();

    QGridLayout *mainLayout;
    QLabel *titleLabel;
    QListWidget *mainMenuList;
    QListWidget *subMenuList;
    QStackedWidget *contentStack;
    QStatusBar *statusBar;
    LoginPage *loginPage;
    QStackedWidget *mainStack;
};

#endif // MAINUIWINDOW_H
