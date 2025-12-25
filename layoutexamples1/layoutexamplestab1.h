#ifndef LAYOUTEXAMPLESTAB_H
#define LAYOUTEXAMPLESTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QListWidget>
#include <QScrollArea>
#include <QLabel>
#include <QPixmap>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

class LayoutExamplesTab : public QWidget
{
    Q_OBJECT
public:
    explicit LayoutExamplesTab(QWidget *parent = nullptr);

private:
    void createGridLayout();
    void createFormLayout();
    void createHorizontalSplitLayout();
    void createVerticalSplitLayout();
    void createHorizontalImageTextLayout();
    void createVerticalImageTextLayout();
    
    QVBoxLayout *mainLayout;
};

#endif // LAYOUTEXAMPLESTAB_H