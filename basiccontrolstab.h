#ifndef BASICCONTROLSTAB_H
#define BASICCONTROLSTAB_H

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QToolButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QSlider>
#include <QLabel>

class BasicControlsTab : public QWidget
{
    Q_OBJECT
public:
    explicit BasicControlsTab(QWidget *parent = nullptr);

private:
    void createButtonGroup();
    void createInputGroup();
    void createSelectionGroup();
    
    QVBoxLayout *mainLayout;
};

#endif // BASICCONTROLSTAB_H