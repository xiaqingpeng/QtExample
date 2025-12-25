#ifndef LAYOUTEXAMPLESTAB3_H
#define LAYOUTEXAMPLESTAB3_H

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

class LayoutExamplesTab3 : public QWidget
{
    Q_OBJECT
public:
    explicit LayoutExamplesTab3(QWidget *parent = nullptr);

private:
    void createDashboardLayout();
    void createMultiPanelLayout();
    void createCollapsiblePanelLayout();
    void createResponsiveFormLayout();
    
    QVBoxLayout *mainLayout;
};

#endif // LAYOUTEXAMPLESTAB3_H