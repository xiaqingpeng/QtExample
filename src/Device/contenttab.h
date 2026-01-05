#ifndef CONTENTTAB_H
#define CONTENTTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "networkmanager.h"
#include "theme_manager.h"

class ContentTab : public QWidget
{
    Q_OBJECT

public:
    ContentTab(QWidget *parent = nullptr);
    ~ContentTab();

private slots:
    void applyTheme();

private:
    NetworkManager *m_networkManager;
    QLabel *m_titleLabel;
    
    void setupUI();
};

#endif // CONTENTTAB_H