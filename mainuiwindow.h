#ifndef MAINUIWINDOW_H
#define MAINUIWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStatusBar>
#include <QGridLayout>

class MainUIWindow : public QWidget
{
    Q_OBJECT

public:
    MainUIWindow(QWidget *parent = nullptr);

private slots:
    void onMainMenuClicked(QListWidgetItem *item);
    void onSubMenuClicked(QListWidgetItem *item);

private:
    void setupUI();
    void setupMainMenu();
    void setupSubMenu(const QString &mainMenu);
    void setupContent();

    QGridLayout *mainLayout;
    QLabel *titleLabel;
    QListWidget *mainMenuList;
    QListWidget *subMenuList;
    QStackedWidget *contentStack;
    QStatusBar *statusBar;
};

#endif // MAINUIWINDOW_H
