#ifndef MAINUIWINDOW_H
#define MAINUIWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QStatusBar>

class MainUIWindow : public QWidget
{
    Q_OBJECT

public:
    MainUIWindow(QWidget *parent = nullptr);

private:
    void setupUI();

    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QTabWidget *tabWidget;
    QStatusBar *statusBar;
};

#endif // MAINUIWINDOW_H
