#ifndef LAYOUTEXAMPLESTAB2_H
#define LAYOUTEXAMPLESTAB2_H

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

class LayoutExamplesTab2 : public QWidget
{
    Q_OBJECT
public:
    explicit LayoutExamplesTab2(QWidget *parent = nullptr);

private:
    void createComplexGridLayout();
    void createNestedLayout();
    void createResponsiveLayout();
    void createCardLayout();
    
    QVBoxLayout *mainLayout;
};

#endif // LAYOUTEXAMPLESTAB2_H