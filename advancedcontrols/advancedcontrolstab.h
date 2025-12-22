#ifndef ADVANCEDCONTROLSTAB_H
#define ADVANCEDCONTROLSTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QProgressBar>
#include <QLCDNumber>
#include <QDial>
#include <QFormLayout>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QCalendarWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QLabel>

class AdvancedControlsTab : public QWidget
{
    Q_OBJECT
public:
    explicit AdvancedControlsTab(QWidget *parent = nullptr);

private:
    void createProgressGroup();
    void createDateTimeGroup();
    void createSplitterGroup();
    
    QVBoxLayout *mainLayout;
};

#endif // ADVANCEDCONTROLSTAB_H